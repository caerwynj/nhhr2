#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

#define DATA "this is just some test"


/* This program creates a socket and initiates a connection with the
 * socket given in the command line. Some data are sent over the
 * connection and then the socket is closed, ending the connection.
 * The form of the command line is: streamwrite hostname portnumber
 */
int main(int argc, char *argv[])
{
  int sock;
  struct sockaddr_in server;
  struct hostent *hp, *gethostbyname();
  char buf[1024];

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
  if (write(sock, DATA, sizeof DATA) == -1)
     perror("writing on stream socket");
  close(sock);
  exit(0);
}
 
