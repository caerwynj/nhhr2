#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

#define DATA "this is just some test"


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
 
