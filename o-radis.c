#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "qdb.h"

int main(void) 
{ 
	char buf[1024], ch[1];
	FILE *tel_in, *tel_out;
	int rval, i, newsocfd, len;
	int descriptorpair[2];
	char prompt='>';
	queuedb db;
	struct sembuf sop;
	struct stat socks;

	pipe(descriptorpair);
	dup2(descriptorpair[1], 1); /* stdout for telnet process */
	close(descriptorpair[1]);

	get_db_record("radis", &db);
	for(i=0;fstat(i,&socks)!=-1;i++) {
		if(S_ISSOCK(socks.st_mode) && i!=db.socfd) 
			close(i);
	}
	tel_in=popen("telnet radis","w");  /* get telnet process going */
	sleep(5);
  /* simple login */
	fprintf(tel_in,"radis\n");
	sleep(1);
	fprintf(tel_in,"97\n");
	sleep(1);
	fprintf(tel_in,"y\n");
	sleep(1);
	fprintf(tel_in,"sc\n");
	sleep(1);
	fprintf(tel_in,"HEALTH1\n");
	sleep(1);
	fprintf(tel_in,"x\n");
	sleep(1);
	fprintf(tel_in,"RADIS\n");
	fflush(tel_in);
	fflush(stdout);

	fcntl(descriptorpair[0], F_SETFL, O_NONBLOCK);
	fflush(stdout);
/* clear the garbage out of the pipe */
	for(i=0;i<10;i++) {
		do {
			fflush(stderr);
			rval=read(descriptorpair[0],ch, 1);
			if (rval == -1) {
				if(errno==EAGAIN)
					break;
				else
					perror("radis");
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
		if ((newsocfd=accept(db.socfd,(struct sockaddr*) 0, (int*) 0))<0) {
			printf("accept error %d \n", errno);
			perror("radis");
			break;
		}
		sop.sem_op=1; /* up mutex */
		semop(semid, &sop, 1); 
	
		memset(buf,0,sizeof buf);
		read(newsocfd, buf, 1024); /* read from connecting process */
		if(buf[0]=='-' && buf[1]=='e')
			exit(0);
		if (buf[0]=='-'&& buf[1]=='i') {
			fprintf(tel_in, "FIND.WUI.SJCS\n");
			fflush(tel_in);
			fprintf(tel_in, "%s\n",&buf[3]); /* send to remote host */
			fflush(tel_in); 
			fprintf(tel_in,"\037 A\n");
			fflush(tel_in);
		} else {
			fprintf(tel_in, "%s\n", buf);
			fflush(tel_in);
		}

/* Something here to notify when the end of text has been reached - the only
 * way to be sure we don't miss any thing.
 */
		fprintf(tel_in, "sh -c \"echo \\\\003 \"\n"); 
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
	return 0;
}
