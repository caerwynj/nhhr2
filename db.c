/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "qdb.h"

extern int errno;

int main(int argc, char *argv[]){ 
	char *progname = "db";
	int rval,i;
	int socfd, globsemid;
	char buf[1024]="who";
	char comline[80];
	char serv_name[20] = "passerv";
	struct sockaddr serv_addr;
	queuedb qdb;
	struct sembuf sop;

/* better error checking needed here ie. usage() */	
	if(argc<2) {
		fprintf(stderr, "Usage: db dbname dbcommand ...\n");
		exit(3);
	}
	globsemid=get_semid();
	sop.sem_num=FILELOCK;
	sop.sem_op=0;
	sop.sem_flg=0;
	semop(globsemid, &sop, 1);
	if(!get_db_record(argv[1], &qdb)) {
		fprintf(stderr, "Database does not exist\n");
		exit(1);
	}
	if(qdb.sess_no == 0) {
		fprintf(stderr, "No sessions open for this database\n");
		exit(2);
	}
	strcpy(serv_name,  qdb.sock_name);
	if(argc>2) strcpy(buf, argv[2]);
	for(i=3;i<argc;i++) {
		strcat(buf, " ");
		strcat(buf, argv[i]);
	}

	serv_addr.sa_family = AF_UNIX;
	strcpy(serv_addr.sa_data, serv_name);

	if ((socfd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) < 0) {
		fprintf(stderr, "socket error %d \n", errno);
		perror(progname);	
		exit(1);
	}
	
 	if (0 != (connect(socfd, &serv_addr, sizeof serv_addr))) {
		fprintf(stderr, "connect error %d \n", errno);
		perror(progname);
		exit(2);
	}
		

	write(socfd, buf, sizeof buf);
	do {
		memset(buf,0,sizeof buf);
		if ((rval = read(socfd, buf, (sizeof buf)-1)) == -1)
			perror("error reading stream message");
		else
			fprintf(stdout, "%s",buf);
		fflush(stdout);
	} while (rval !=0); 
	close (socfd);	
	return 0;
}

 
