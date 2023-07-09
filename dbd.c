/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* implemtation of dbdaemo: interface */

#include "dbd.h"
#include "qdb.h"
#include "pid_table.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

char * progname="dbdaemon";
int openqueue(char * queuename);

int open_session(int argc, char *argv[]) 
{
/* fork a new session connected to dbname */
	queuedb db;
	char string[15]="/var/tmp/";	
	int process;

	if(!get_db_record(argv[1], &db)) {
		return 0;
	}

	(db.sess_no)++;  
	update_db_record(argv[1], &db);
	
	switch (process=fork())	{
		case -1: /* Fork failed: report error */ 
			return 0;
			break;
		case 0: /* This is the child */
			if(!strcmp("pas", argv[1]))
				execv("/home/cjones/bin/pas", &argv[1]);
			else
				execv("/home/cjones/bin/path", &argv[1]);
			break;
		default: /* This is the parent */
			add_pid_entry(argv[1], process);
			return 1;
			break;
	}
}

int close_session(char * dbname)
{
	int rval, socfd;
	queuedb db;
	char buf[]="-e", sockname[20];
	unsigned pid, status;

	if(!get_db_record(dbname, &db))
		return 0; /* database doesn't exit */
	if(db.sess_no == 0)
		return 0; /* no sessions running */
	strcpy(sockname, db.sock_name);
	rval=get_n_pid_entries(dbname);
	if(rval==1) {  /* change the server name to null */
		       /* no more processes can access it then */
		       /* should use a semaphore here */
		strcpy(db.sock_name, "NULL");
		update_db_record(dbname, &db);
	}
	/* connect to socket and send it a command to exit */
	socfd=get_sock(sockname);
	write(socfd, buf, sizeof buf);
	close(socfd);
	pid=wait(&status);
	if(!remove_pid_entry(pid))
		fprintf(stderr, "couldn't remove pid entry");
	return 1;
}

int kill_session(char * dbname)
{
/* We need to reset the semaphores when we kill a process - the best
   way I can think of is to kill all sessions and then reset the sems
*/
	int rval,status,count=0;
	queuedb db;
	union semun arg;

	get_db_record(dbname, &db);
	db.sess_no=0;
	update_db_record(dbname, &db);
	while(rval=kill1_pid(dbname)) {
		wait(&status);
		count++;
	}
	arg.val=1;
	semctl(semid, db.semnums[MUTEX], SETVAL, arg); 
	return count;
}

int dbd_init(int maxdb)
{
/* create a semaphore set and initialise all its values */
/* create the qdb file */
	key_t key;
	int nsems=3; 				  
	union semun mysemun;

	nsems = maxdb*nsems+1;
	maxsems =  nsems;
	init_db_file();
	if ((key = ftok("/home/cjones/diary", 'z'))<0) {
		perror("dbd_init");
		exit(1);
	}

	if ((semid = semget(key, nsems, IPC_CREAT | 0666))<0) {
		perror("dbd_init");
		exit(2); 
	}
	
	/* semnum 0 is the file lock for the qdb file */
	mysemun.val=0;	
	if ((semctl(semid, FILELOCK, SETVAL, mysemun))<0) {
		perror("dbd_init");
		exit(3); 
	}

	return 0;
}

int new_db(char * dbname)
{
/* put a new record in the qdb file */
/* initialise its semaphores (if there's room) */
/* create its socket queue */
	queuedb db;
	int i, socfd, sems3[3];
	char sockname[15]="/var/tmp/";
	union semun arg;

	strncat(sockname, dbname,3);
	strcat(sockname, "v");

	if (get_db_record(dbname, &db)) {
		return 0; /* database already exists */
	}
	else {
		socfd=openqueue(sockname);
		db.socfd=socfd;
		if(!get_free_sems(sems3))
			fprintf(stderr, "error getting free sems\n");
		for(i=0;i<3;i++) db.semnums[i]=sems3[i];
		strcpy(db.sock_name, sockname);
		strcpy(db.db_name, dbname);
		db.sess_no=0;
		put_db_record(&db);
		arg.val=1;
		semctl(semid, sems3[0], SETVAL, arg); 
	}
	return 1;
}

int delete_db(char * dbname)
{
	queuedb db;

/* remove the relevant record from the qdb file */
	if(get_db_record(dbname, &db)) {
		unlink(db.sock_name);
		remove_db_record(dbname);
		kill_session(dbname);
		close(db.socfd);
		return 1;
	} else {
		return 0;
	}
}

void session_info(void)
{
/* return semaphore info ie. the queue sizes for each database */
	print_pid_table();
}

/*************************************************************************
 * Utilities, not part of the interface.                                 *
 *************************************************************************/

int openqueue(char * queuename) 
{ 
	int socfd; 
	struct sockaddr serv_addr;

	serv_addr.sa_family = AF_UNIX;
	strcpy(serv_addr.sa_data, queuename);

	socfd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC); 
	if (socfd == -1 ) {
		perror("socket error");	
		exit(1);
	}

	if (bind(socfd, &serv_addr, sizeof serv_addr)!=0) {
		perror("bind error");
		exit(1);
	}
	chmod(queuename, 0666);
	listen(socfd, 5);

	return socfd;
}

int get_sock(char * serv_name)
{ 
	
	struct sockaddr serv_addr;
	int socfd;

	serv_addr.sa_family = AF_UNIX;
	strcpy(serv_addr.sa_data, serv_name);

	if ((socfd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) < 0) {
		printf("socket error %d \n", errno);
		perror("get_sock");	
		exit(1);
	}
	
 	if (0 != (connect(socfd, &serv_addr, sizeof serv_addr))) {
		printf("connect error %d \n", errno);
		perror("get_sock");
		exit(2);
	}
	return socfd;
}
