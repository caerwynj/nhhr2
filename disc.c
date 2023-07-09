/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>

int main(void)
{

	char string[1024];

	while (gets(string)) {
		if (strchr(string, '|'))
			puts(string);
	}
	return 0;
}
