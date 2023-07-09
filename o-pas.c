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

int main(void) { 

	char buf[1024], ch[1];
	FILE *tel_in, *tel_out;
	int rval, i, interactive, newsocfd, len, passoc=5;
	int descriptorpair[2];
	char prompt=':';
	queuedb db;
	struct sembuf sop;
	struct stat socks;

/* This if for debugging - we need to query our user file descriptor table
   to find out what sockets and files we have open - close the unneccessary
   and get the file descriptor of our queue/socket 
*/
	get_db_record("pas", &db);
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
	fprintf(tel_in,"WINTEGRATE\n");  
	fflush(tel_in);

/* set read to non-blocking mode */
	fcntl(descriptorpair[0], F_SETFL, O_NONBLOCK);
	fflush(stdout);

/* clear the garbage out of the pipe */
#ifdef DEBUG
	fprintf(stderr, "Reading garbage ...");
#endif
	for(i=0;i<10;i++) {
		do {
			fflush(stderr);
			rval=read(descriptorpair[0],ch, 1);
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
	fcntl(descriptorpair[0], F_SETFL, 0);
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
		if(buf[0]=='-' && buf[1]=='e')
			exit(0);
		if(buf[0]=='-' && buf[1]=='i') {
			interactive=1;
			fprintf(tel_in,"WUI.DDE\n");
			fflush(tel_in);
			fprintf(tel_in, "%s\n",&buf[3]);
			fflush(tel_in); 
			fprintf(tel_in, "\nEND\n");
		}else {
			interactive=0;
			fprintf(tel_in, "%s\n", buf);
			fflush(tel_in);
		}

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
				if(interactive) {
					i=0;
					len=strlen(buf);
					while(i<len) {
						ch[0]=buf[i++];
						if(isprint(ch[0])||isspace(ch[0]))
							write(newsocfd, ch, 1);
					} 
				} else   write(newsocfd,buf,strlen(buf));
			}
		}
		close(newsocfd);
	}while (1);
	return 0;
}

