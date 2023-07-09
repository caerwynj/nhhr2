/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>
#include <errno.h>

int main(void)
{
	int ch;
	int data = 0;
	int rval;
	
	while((ch=getchar()) != EOF){
		if(ch=='\"') {
			if (data==0)
				data=1;
			else {
				data=0;
				putchar('\n');
			}
		}
		if(data && (ch != '\"'))
			rval = putchar(ch);
		if(rval==EOF)
			perror("put char");
	}
	fflush(stdout);
	return 0;
}

