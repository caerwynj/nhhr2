#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

int openconnect(char * hostname, char * port);
int openqueue(char * queuename); 

extern int errno;
char *progname = "serv";

int main(void) { 

	int socfd, newsocfd, remot_fd;
	char buf[1024];
	struct sockaddr serv_addr;

	socfd = openqueue("tstserv");
	remot_fd = openconnect("localhost", "23");
	logon(remot_fd);

	if ((listen(socfd, 5))<0) {
		printf("listen error %d \n", errno);
		perror(progname);
		exit(1);
	}

	for(;;){
		if ((newsocfd=accept(socfd,(struct sockaddr *) 0, (int *) 0))<0) {
			printf("accept error %d \n", errno);
			perror(progname);
			exit(1);
		}
	
		dup2(newsocfd, 1);
		read(newsocfd, buf, 1024);
		write(remot_fd, buf, 1024);
		read(remot_fd, buf, 1024);
		printf("%s",buf);
		close(newsocfd);
		close(1);
	}
	
	close (socfd);
	if (unlink(serv_name)<0) perror(progname);	
	return 0;
}

 


/* This funciton creates a socket and initiates a connection with the
 * socket given in the argument. The function returns the sockfd. 
 */

int openconnect(char * hostname, char * port)
{
  int sock;
  struct sockaddr_in server;
  struct hostent *hp, *gethostbyname();

  /* Create socket */ 
  sock = socket( AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
     perror("opening stream socket");
     exit(1);
  }
  /* Connect socket using name specified by the command line. */
  server.sin_family = AF_INET;
  hp = gethostbyname(argv[1]);
/*
 * gethostbyname returns a structure including the network address
 * of the specified host.
 */
  if (hp == (struct hostent *) 0) {
     fprintf(stderr, "%s: unknown host\n", argv[1]);
     exit(2);
  }
  memcpy((char *) &server.sin_addr, (char *) hp->h_addr,
	 hp->h_length);
  server.sin_port = htons(atoi( argv[2]));
  if (connect(sock, (struct sockaddr *) &server, sizeof server) == -1) {
     perror("connecting stream socket");
     exit(1);
  }
  return sock;
}
 

int openqueue(char * queuename) 
{ 

	int socfd; 
	struct sockaddr serv_addr;

	serv_addr.sa_family = AF_UNIX;
	strcpy(serv_addr.sa_data, queuename);

	socfd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) < 0); 
	if ((socfd == -1 ) {
		perror("socket error");	
		exit(1);
	}

	if ((bind(socfd, &serv_addr, sizeof serv_addr)!=0) {
		perror("bind error");
		exit(1);
	}

	return socfd;
}

 
