#include "pid_table.h"
#include <stdio.h>

int main(void) 
{
	int rval;

	if(get_pid("pas")==0)
		printf("No entries yet\n");
	add_pid_entry("pas", 7777);
	add_pid_entry("pas", 8888);
	rval=get_n_pid_entries("pas");
	printf("Number of pid entries is %d\n",rval);
	kill1_pid("pas");
	rval=get_pid("pas");
	printf("PID of pas is %d\n",rval);
	rval=kill_all_pid("pas");
	printf("Killed %d processes\n",rval);

	return 0;
}
