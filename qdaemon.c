/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* check if Q already running
 * if not fork Q and exit
 * simple !
*/

#include <unistd.h>

int main(void)
{
	int process;

	switch (process=fork()) {
		case 0:	execl("Q", (char *) 0);
			break;

		case -1: /*error*/
			break;
		default: exit(0);
	}
	return 0;
}
