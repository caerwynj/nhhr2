/* (c) 1996 Nevill Hall & District NHS Trust, I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>
#include <tcl/tcl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

extern int openqueue(char *queuename);
int dbd_tcl_init(Tcl_Interp * interp);

int main(void)
{
	Tcl_Interp *interp;
	int code, socfd, newsocfd;
	char buf[1024];
	char ch = 4;

	interp = Tcl_CreateInterp();
	if (dbd_tcl_init(interp) == TCL_ERROR)
		exit(1);

	socfd = openqueue("/var/tmp/rend");
	do {
		memset(buf,0, sizeof buf);
		newsocfd = accept(socfd, (struct sockaddr *) 0, (int *) 0);
		read(newsocfd, buf, sizeof buf);
	
		code = Tcl_GlobalEval(interp, buf);
		if (*interp->result != 0) {
			write(newsocfd, interp->result, strlen(interp->result));
		}
		write(newsocfd, &ch, 1);
		close(newsocfd);
	} while (1);
}
