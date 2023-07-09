#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define TRUE 1

const char * I_PROG = "WUI.DDE";
const char * E_PROG_CMD = "END";
const char * SH_CMD = "sys echo \\\\003";

extern int errno;
char *progname = "serv";

int main(void) { 

	int socfd, backlog=5, cli_addrlen=0, newsocfd;
	char buf[1024];
	char *serv_name = "passerv";
	struct sockaddr serv_addr, clientaddr;
	FILE *tel_in, *tel_out;
	int rval,i=1, interactive=0;
	int descriptorpair[2];
	char prompt=':';
	char ch[1];
	sembuf sop;

	pipe(descriptorpair);
	dup2(descriptorpair[1], 1); /* stdout for telnet process */
	close(descriptorpair[1]);
	
	tel_in=popen("telnet nhpas","w");  /* get telnet process going */
	sleep(5);

	list[0] = "WINTEGRATE"; 
	send_cmds(1, list);

	fflush(stdout);
	flush_pipe(descriptorpair[0]);

	tel_out=fdopen(descriptorpair[0],"r");

	get_db_record(dbname, &dbrecord);
	sop.sem_num = dbrecord.semnum[0];
	sop.sem_flg = 0;

	do{
	
		sop.sem_op = -1; /* down mutex */
		if (semop(semid, sop, 1)<0) {
			perror(progname);
			exit(1);
		}
		if ((newsocfd=accept(dbrecord.socfd, &clientaddr, &cli_addrlen))<0) {
			printf("accept error %d \n", errno);
			perror(progname);
			break;
		}
		sop.sem_op = 1; /* up mutex */
		if (semop(semid, sop, 1)<0) {
			perror(progname);
			exit(1);
		}
	
		memset(buf,0,sizeof buf);
		read(newsocfd, buf, 1024); /* read from connecting process */
		if(buf[0]=='-' && buf[1]=='i') {
			interactive=1;
			fprintf(tel_in,"WUI.DDE\n");
			fflush(tel_in);
			fprintf(tel_in, "%s\n",&buf[3]); /* send to remote host */
			fflush(tel_in); 
			fprintf(tel_in, "END\n");
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
		redirect_data(prompt, tel_out, newsocfd, interactive);
		close(newsocfd);
	}while (TRUE);
	
	close (socfd);
	pclose(tel_in);
	if (unlink(serv_name)<0) perror(progname);	
	return 0;
}

void send_cmds(int len, char * list[]) {
	int i;
	for(i=0;i<len;i++) {
		fprintf(tel_in,list[i]);  /* simple login */
		fprintf(tel_in, "\n");
		fflush(tel_in);
	}
}

void redirect_data(char prompt,  FILE * tel_out, int newsocfd, int interactive) 
{
/* Loop until end of text is reached (ch = 3) */		
/* also possible to loop until prompt returns but this is unreliable */
	int i;
	char ch[1];
	char buf[1024];

	while(!strchr(fgets(buf, 1024, tel_out), 3)) {
		if (buf[0] == prompt)
			; /* discard it */
		else { 
			if(interactive) {
				i=0;
				while(i<sizeof buf) {
					ch[0]=buf[i++];
					if(isprint(ch[0])||isspace(ch[0]))
						write(newsocfd, ch, 1);
				} 
			} else write(newsocfd,buf,strlen(buf));
		}
	}
}
void flush_pipe(int fd /*descriptorpair[0]*/) {
/* clear the garbage out of the pipe */
	int i, rval;

	fcntl(fd, F_SETFL, O_NONBLOCK);
	for(i=0;i<10;i++) {
		do {
			rval=read(fd,ch, 1);
			if (rval == -1) {
				if(errno==EAGAIN)
					break;
				else
					perror("Emptying pipe");
			}
		}while (rval != -1);
	sleep(1);
	}
	fcntl(fd, F_SETFL, 0);
}

int get_semid() 
{
	key_t key;
	char * pathname= "/home/cjones/diary";
	char proj= 'A';
	int semid;
	union semun mysemun;


	if ((key = ftok(pathname, proj))<0) {
		perror(progname);
		exit(1);
	}
	
	if ((semid = semget(key, 0, 0x666))<0) {
		perror(progname);	
		exit(2);
	}
	
	return semid;
}
