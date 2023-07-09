/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

int main(int argc, char *argv[]){ 
	char *progname = "db";
	int rval,i;
	int socfd;
	char buf[1024]="q_restart "; 
	char serv_name[20] = "/var/tmp/rend";
	struct sockaddr serv_addr;
	char ch;

	if(argc == 2) {
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
		strcat(buf, argv[1]);
		write(socfd, buf, strlen(buf));
		return 0;	
	}

	printf("Q>");
	while(fgets(buf, sizeof buf, stdin)) { 
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

		write(socfd, buf, strlen(buf));
		memset(buf, 0, sizeof buf);
		for(i=0;buf[i]!=4;i++) {
			rval=read(socfd, &buf[i], 1);
			if(rval < 0)
				break;
			printf("%c",buf[i]);
		}
		if(strlen(buf)>1)
			printf("\n");
		printf("Q>");
		fflush(stdout);
		close (socfd);	
	}
	return 0;
}

 
