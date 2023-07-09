
int openqueue(char * queuename) { 

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
	listen(socfd, 5);

	return socfd;
}

 
