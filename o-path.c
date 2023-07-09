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

int main(void) 
{ 
	char buf[1024], ch[1];
	FILE *tel_in, *tel_out;
	int rval, i, newsocfd, len;
	int descriptorpair[2];
	char prompt=':';
	queuedb db;
	struct sembuf sop;
	struct stat socks;

	get_db_record("path", &db);
	for(i=0;fstat(i,&socks)!=-1;i++) {
		if(S_ISSOCK(socks.st_mode) && i!=db.socfd) 
			close(i);
	}
	pipe(descriptorpair);
	dup2(descriptorpair[1], 1); /* stdout for telnet process */
	close(descriptorpair[1]);
	tel_in=popen("telnet path","w");  /* get telnet process going */
	sleep(5);
	fprintf(tel_in,"scrandon\n");  /* simple login */
	fflush(tel_in);
	sleep(1);
	fprintf(tel_in,"comp1le\n");
	fflush(tel_in);

	fcntl(descriptorpair[0], F_SETFL, O_NONBLOCK);
	fflush(stdout);
/* clear the garbage out of the pipe */
#ifdef DEBUG
	fprintf(stderr, "Reading garbage ");
#endif
	for(i=0;i<10;i++) {
		do {
			fflush(stderr);
			rval=read(descriptorpair[0],ch, 1);
			if (rval == -1) {
				if(errno==EAGAIN)
					break;
				else
					perror("path");
			}
		}while (rval != -1);
	sleep(1);
	}
	fcntl(descriptorpair[0], F_SETFL, 0);
	tel_out=fdopen(descriptorpair[0],"r");

	semid=get_semid();
	sop.sem_num=db.semnums[MUTEX];
	sop.sem_flg=0;
	do{
		sop.sem_op=-1; /* down mutex */
		semop(semid, &sop, 1);
		if ((newsocfd=accept(db.socfd,(struct sockaddr*) 0,(int*) 0))<0) {
			printf("accept error %d \n", errno);
			perror("path");
			break;
		}
		sop.sem_op=1; /* up mutex */
		semop(semid, &sop, 1);
	
		memset(buf,0,sizeof buf);
		read(newsocfd, buf, 1024); /* read from connecting process */
		if ((buf[0]=='-') && (buf[1] == 'e')) 
			exit(0);
		if ((buf[0]=='-') && (buf[1] == 'i')) {
			fprintf(tel_in, "SJCS.PATH\n");
			fflush(tel_in);
			fprintf(tel_in, "%s\n", &buf[3]);
			fflush(tel_in);
			sleep(5);
			fprintf(tel_in, "\003");
			fprintf(tel_in, "END\n");
		}else{
		fprintf(tel_in, "%s\n",buf); /* send to remote host */
		fflush(tel_in); 
		}

/* Something here to notify when the end of text has been reached - the only
 * way to be sure we don't miss any thing.
 */
		fprintf(tel_in, "!echo \\\\003 \n"); 
		fflush(tel_in);
		fflush(stdout);

/* Loop until end of text is reached (ch = 3) */		
/* also possible to loop until prompt returns but this is unreliable */
		while(!strchr(fgets(buf, 1024, tel_out), 3)) {
			if (buf[0] == prompt)
				; 
			else { 
				i=0;
				len=strlen(buf);
				while(i< len) {
					ch[0]=buf[i++];
					if(isprint(ch[0])||isspace(ch[0]))
						write(newsocfd, ch, 1);
				}
			}   
		}
		close(newsocfd);
	}while (1);
	return 0;
}
