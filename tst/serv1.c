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

extern int errno;
char *progname = "serv";

int main(void) { 

	int socfd, backlog=5, cli_addrlen=0, newsocfd;
	char buf[1024];
	char *serv_name = "tstserv";
	struct sockaddr serv_addr, clientaddr;
	FILE *tel_in, *tel_out;
	int rval,i=1;
	int descriptorpair[2];
	char * prompt="nevroute:~$";
	char ch[1];

	pipe(descriptorpair);
	dup2(descriptorpair[1], 1); /* stdout for telnet process */
	close(descriptorpair[1]);
	
	tel_in=popen("telnet -a localhost","w");  /* get telnet process going */
	sleep(5);
	fprintf(tel_in,"cjones\n");  /* simple login */
	fflush(tel_in);

/* clear the garbage out of the pipe */
	do {
	memset(buf,0,sizeof buf);
	rval=read(descriptorpair[0],buf, 1024);
	for(i=0;i<rval;i++)
		if(isprint(buf[i]))
		fputc(buf[i], stderr);
	}while (!strstr(buf,prompt));

	tel_out=fdopen(descriptorpair[0],"r");

	serv_addr.sa_family = AF_UNIX;
	strcpy(serv_addr.sa_data, serv_name);

	socfd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC); 
	if (socfd == -1 ) {
		printf("socket error %d \n", errno);
		perror(progname);	
		exit(1);
	}

	if ((bind(socfd, &serv_addr, sizeof serv_addr))!=0) {
		printf("bind error %d \n", errno);
		perror(progname);
		exit(1);
	}

	if ((listen(socfd, backlog))<0) {
		printf("listen error %d \n", errno);
		perror(progname);
		exit(1);
	}

	do{
		if ((newsocfd=accept(socfd, &clientaddr, &cli_addrlen))<0) {
			printf("accept error %d \n", errno);
			perror(progname);
			break;
		}
	
		memset(buf,0,sizeof buf);
		read(newsocfd, buf, 1024); /* read from connecting process */
		fprintf(tel_in, "%s\n",buf); /* send to remote host */
		fflush(tel_in); 
		fprintf(tel_in, "echo -e \"\\003\" \n");
		fflush(tel_in);

		fflush(stdout);
/*		printf("\n\003");
		fflush(stdout);
*/
		while(!strchr(fgets(buf, 1024, tel_out), 3)) {
			if (strstr(buf, prompt))
				; /* discard it */
			else
				write(newsocfd, buf, strlen(buf));
		}

/*		while((ch[0]=fgetc(tel_out))!=3)
			write(newsocfd, ch, 1);
*/

/*		do{
		read(descriptorpair[0], ch, 1);
		write(newsocfd, ch, 1);
		}while(ch[0] != 3);
*/
		close(newsocfd);
	}while (TRUE);
	
	close (socfd);
	pclose(tel_in);
	if (unlink(serv_name)<0) perror(progname);	
	return 0;
}

