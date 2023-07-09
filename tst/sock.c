#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "qdb.h"

extern int errno;

int main(int argc, char *argv[]){ 
	char *progname = "db";
	int rval,i;
	int socfd;
	char buf[1024]="who";
	char comline[80];
	char serv_name[20] = "passerv";
	struct sockaddr serv_addr;
	queuedb qdb;

/* better error checking needed here ie. usage() */	

	get_db_record(argv[1], &qdb);
	strcpy(serv_name,  qdb.sock_name);
	if(argc>2) strcpy(buf, argv[2]);
	for(i=3;i<argc;i++) {
		strcat(buf, " ");
		strcat(buf, argv[i]);
	}

	serv_addr.sa_family = AF_UNIX;
	strcpy(serv_addr.sa_data, serv_name);

	if ((socfd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) < 0) {
		printf("socket error %d \n", errno);
		perror(progname);	
		exit(1);
	}
	
 	if (0 != (connect(socfd, &serv_addr, sizeof serv_addr))) {
		printf("connect error %d \n", errno);
		perror(progname);
		exit(2);
	}
		

	write(socfd, buf, sizeof buf);
	do {
		memset(buf,0,sizeof buf);
		if ((rval = read(socfd, buf, (sizeof buf)-1)) == -1)
			perror("error reading stream message");
/*		if (rval == 0)
			printf("Ending connection\n");
*/		else
			printf("%s",buf);
		fflush(stdout);
	} while (rval !=0); 
	close (socfd);	
	return 0;
}

 
