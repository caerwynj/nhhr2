/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* The pas login and session application for use with a qserver, intended for
 * WWW intranet access
 */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include "qdb.h"

void garbage_man(int fd);

int main(int argc, char * argv[]) { 

	char buf[1024], ch[1];
	FILE *tel_in, *tel_out;
	int rval, i, newsocfd, len, cmdcnt;
	int descriptorpair[2];
	char prompt=':';
	queuedb db;
	struct sembuf sop;
	struct stat socks;

/* This if for debugging - we need to query our user file descriptor table
   to find out what sockets and files we have open - close the unneccessary
   and get the file descriptor of our queue/socket 
*/
	get_db_record(argv[0], &db);
	for(i=0;fstat(i,&socks)!=-1;i++) {
		if(S_ISSOCK(socks.st_mode) && i!=db.socfd) 
			close(i);
	}

	pipe(descriptorpair);
	dup2(descriptorpair[1], 1); /* stdout for telnet process */
	close(descriptorpair[1]);   /* release file descriptor */
	
/* get telnet process going */
	tel_in=popen("telnet nhpas","w");  
	sleep(5);

/* simple login */
	if (argc > 1) {
		for(i=1;i<argc;i++) {
			fprintf(tel_in,"%s\n",argv[i]);  
			fflush(tel_in);
			sleep(1);
			memset(argv[i], 0,strlen(argv[i]));
		}
	} else {
		fprintf(tel_in,"WINTEGRATE\n");  
		fflush(tel_in);
	}

	garbage_man(descriptorpair[0]);
	tel_out=fdopen(descriptorpair[0],"r");

/* get semaphore no. and socket file descriptor */
	semid=get_semid();
	sop.sem_num=db.semnums[MUTEX];
	sop.sem_flg=0;

/* infinite loop collecting next command from socket queue */
	do{
/* critical section */
		sop.sem_op=-1;  /* down mutex */
		semop(semid, &sop, 1);
		if ((newsocfd=accept(db.socfd,(struct sockaddr *) 0,(int *) 0))<0) {
			fprintf(stderr,"accept error %d \n", errno);
			perror("pas");
			exit(1);
		}
		sop.sem_op=1; /* up mutex */
		semop(semid, &sop, 1);
/* end of critical section */
	
		memset(buf,0,sizeof buf);
		read(newsocfd, buf, 1024); /* read from connecting process */
		strcat(buf,"  END ");
/* Change each space char to a newline */
		cmdcnt=1;
		for(i=0;buf[i];i++) {
			if(buf[i]==' ') {
				buf[i]='\n';
				cmdcnt++;
			}
		}
		if ((buf[0]=='-') && (buf[1] == 'e')) 
			exit(0);
		fprintf(tel_in,"WUI.DDE\n");
		fflush(tel_in);
		fprintf(tel_in, "%s\n", buf);

/* Something here to notify when the end of text has been reached - the only
 * way to be sure we don't miss any thing.
 */
		fprintf(tel_in, "sys echo \\\\003 \n"); 
		fflush(tel_in);
		fflush(stdout);

/* Loop until end of text is reached (ch = 3) */		
/* also possible to loop until prompt returns but this is unreliable */
		while(!strchr(fgets(buf, 1024, tel_out), 3)) {
			if (buf[0] == prompt)
				; /* discard it */
			else { 
				i=0;
				len=strlen(buf);
				while(i<len) {
					ch[0]=buf[i++];
					if(isprint(ch[0])||isspace(ch[0]))
						write(newsocfd, ch, 1);
				} 
			}
		}
		close(newsocfd);
	}while (1);
}

void garbage_man(int fd) 
{
	int i, rval;
	char ch;

/* set read to non-blocking mode */
	fcntl(fd, F_SETFL, O_NONBLOCK);
	fflush(stdout);

/* clear the garbage out of the pipe */
#ifdef DEBUG
	fprintf(stderr, "Reading garbage ...");
#endif
	for(i=0;i<10;i++) {
		do {
			fflush(stderr);
			rval=read(fd,&ch, 1);
			if (rval == -1) {
				if(errno==EAGAIN)
					break;
				else
					perror("pas");
			}
		}while (rval != -1);
	sleep(1);
	}

/* set read back to blocking mode */
	fcntl(fd, F_SETFL, 0);
}
