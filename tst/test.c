#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int openconnect(char * hostname, char * port);
int logon(int sockfd);

int main(void)
{
	int remote_fd;

	remote_fd = openconnect("localhost", "13");
	printf("connected to localhost\n");
	logon(remote_fd);
	close(remote_fd);
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
  hp = gethostbyname(hostname);
/*
 * gethostbyname returns a structure including the network address
 * of the specified host.
 */
  if (hp == (struct hostent *) 0) {
     fprintf(stderr, "%s: unknown host\n", hostname);
     exit(2);
  }
  memcpy((char *) &server.sin_addr, (char *) hp->h_addr,
	 hp->h_length);
  server.sin_port = htons(atoi(port));
  if (connect(sock, (struct sockaddr *) &server, sizeof server) == -1) {
     perror("connecting stream socket");
     exit(1);
  }
  return sock;
}
 
int logon(int sockfd)
{
	char buf[1024];
	int MAXSIZE = 1024;
	int rval;

	memset(buf, 0, sizeof buf);
	if ((rval = read(sockfd, buf, MAXSIZE)) == -1)
		perror("reading stream message");
	if (rval == 0)
		printf("Ending connection\n");
	else
		printf("-->%s\n", buf);

	return 1;
}
