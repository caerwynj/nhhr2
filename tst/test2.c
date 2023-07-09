#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int openqueue(char * name);
int logon(int pipepair[]);

int main(void)
{
	int sock_fd, newsocfd;
	int processid;
	int descriptorpair[2];
	int rval;
	char buf[1024];
	struct sockaddr hostaddr;

	pipe(descriptorpair);
	dup2(descriptorpair[0], 0);
	dup2(descriptorpair[1],1);

	sock_fd = openqueue("tstq");
	switch (processid=fork())
	{
		case -1:
			/* fork failed: report error */
			perror("fork failed");
			exit(1);
		case 0:
			/* This is the child */
			execl("/bin/telnet","telnet", "-a", "localhost", (char *) 0);
			break;
		default:
			/* This is the parent */
			break;
	}
	logon(descriptorpair);
	if((listen(sock_fd, 5))<0) {
		perror("listening");
		exit(1);
	}
	for(;;) {
		newsocfd=accept(sock_fd, &hostaddr ,(int *) 0);
		if (newsocfd== -1) {
			perror("accept");
			exit(1);
		}
		else {
			dup2(newsocfd, descriptorpair[1]);
			memset(buf,0,sizeof buf);
			if((rval=read(newsocfd, buf, sizeof buf)) == -1)
				perror("reading stream message");
			else
				write(descriptorpair[0], buf, sizeof buf);
		}
		close(newsocfd);
	}	
	close(sock_fd);
	return 0;
}

int logon(int pipepair[])
{
	char buf[1024];

/* write logon script to each remote host - system call to sh script */
/* convert fd's to streams */
	memset(buf,0,sizeof buf);
	strcpy(buf,"cjones
");
	write(pipepair[0],buf, strlen(buf)); 
	read(pipepair[1],buf, sizeof buf);
	fprintf(stderr, "%s", buf);
	return 1;
	/* return true if all was success */
}

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

	if ((bind(socfd, &serv_addr, sizeof serv_addr))!=0) {
		perror("bind error");
		exit(1);
	}

	return socfd;
}

 
