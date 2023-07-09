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
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include "qdb.h"

void garbage(int fd);

int main(int argc, char *argv[]) 
{ 
	char buf[1024], ch;
	FILE *tel_in, *tel_out;
	int rval, i, j, newsocfd, len, cmdcnt;
	int descriptorpair[2];
	queuedb db;
	struct sembuf sop;
	struct stat socks;

	get_db_record(argv[0], &db);
	for(i=0;fstat(i,&socks)!=-1;i++) {
		if(S_ISSOCK(socks.st_mode) && i!=db.socfd) 
			close(i);
	}
	pipe(descriptorpair);
	dup2(descriptorpair[1], 1); /* stdout for telnet process */
	close(descriptorpair[1]);

	strcpy(buf,"telnet ");
	tel_in=popen(strcat(buf,argv[0]),"w");  /* get telnet process going */

/* login to wui account */
	sleep(5);
	if (argc > 1) {
		for(i=1;i<argc;i++) {
			fprintf(tel_in,"%s\n",argv[i]);  
			fflush(tel_in);
			sleep(1);
			memset(argv[i], 0, strlen(argv[i]));
		}
	} else {
		fprintf(stderr, "Not enough args\n");
		exit(1);
	}

	garbage(descriptorpair[0]);
	tel_out=fdopen(descriptorpair[0],"r");

	semid=get_semid();
	sop.sem_num=db.semnums[MUTEX];
	sop.sem_flg=0;
	do{
		sop.sem_op=-1; /* down mutex */
		semop(semid, &sop, 1);
		if ((newsocfd=accept(db.socfd,(struct sockaddr*) 0,(int*) 0))<0) {
			fprintf(stderr, "accept error %d \n", errno);
			perror("session");
			break;
		}
		sop.sem_op=1; /* up mutex */
		semop(semid, &sop, 1);
	
		memset(buf,0,sizeof buf);
		read(newsocfd, buf, 1024); /* read from connecting process */
		if ((buf[0]=='-') && (buf[1] == 'e')) 
			exit(0);
		strcat(buf," END");
/* Change each space char to a newline */
		cmdcnt=1;
		for(i=0;buf[i];i++) {
			if(buf[i]==' ') {
				buf[i]='\n';
				cmdcnt++;
			}
		}
		fprintf(tel_in, "%s\n",buf); 
		fflush(tel_in); 
		fflush(stdout);

/* Loop until end of text is reached (ch = 3) */		
/* also possible to loop until prompt returns but this is unreliable */
		while(cmdcnt--){
			ch=fgetc(tel_out);
			do{
				if(isprint(ch)||isspace(ch))
					write(newsocfd, &ch, 1);
			}while((ch=fgetc(tel_out))!= '?'); 
		}
		close(newsocfd);
	}while (1);
	return 0;
}

void garbage(int fd)
{
	int rval, i;
	char ch;

	fcntl(fd, F_SETFL, O_NONBLOCK);
	fflush(stdout);
/* clear the garbage out of the pipe */
	for(i=0;i<10;i++) {
		do {
			fflush(stderr);
			rval=read(fd,&ch, 1);
			if (rval == -1) {
				if(errno==EAGAIN)
					break;
				else
					perror("session");
			}
		}while (rval != -1);
	sleep(1);
	}
	fcntl(fd, F_SETFL, 0);
}
