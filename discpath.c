/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>
#include <errno.h>

int main(void)
{
	int ch, data = 0;
	int rval, pipefound = 0;
	int dboutput =0;
	
	while((ch=getchar()) != EOF){
		if(ch=='`') {
			if (data==0)
				data=1;
			else {
				data=0;
				if(!pipefound && dboutput) {
	 				putchar('\n');
				}
			}
		}
		else {
			if(data && (ch !='`') && (ch != '\n')) {
				if(isalnum(ch) && ch != '1')
					dboutput=1;
				if(ch =='|') {
					putchar('\n');
					pipefound=1;
				}else if(!dboutput && ch =='1') 
						;
				else
					rval = putchar(ch);
			}
		}
		if(rval==EOF)
			perror("put char");
	}
	fflush(stdout);
	return 0;
}

