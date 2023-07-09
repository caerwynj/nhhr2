#include "dbd.h"
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int status;
	dbd_init(3);
	new_db("pas");
	new_db("path");
	new_db("radis");
	open_session("pas");
	open_session("pas");
	open_session("path");
	open_session("path");
	open_session("radis");
	wait(&status);
	return 0;
}
