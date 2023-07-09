/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

#include <stdio.h>
#include <string.h>
#include <tcl/tcl.h>
#include "dbd.h"

int open_sessionCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) 
{
	int rval;

	if (argc < 2) {
		interp->result = "wrong # of args";
		return TCL_ERROR;
	}
	rval = open_session(argc, argv);
	if(rval == 1) {
		interp->result = "OK";
		return TCL_OK;
	}
	else {
		interp->result = "db doesn't exist";
		return TCL_ERROR;
	}
}

int new_dbCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) 
{
	int rval;
	if (argc != 2) {
		interp->result = "wrong # of args";
		return TCL_ERROR;
	}
	rval = new_db(argv[1]);
	if(rval == 1)
		return TCL_OK;
	else {
		interp->result = "new_db: init hasn't been called or DB exists";
		return TCL_ERROR;
	}
}

int dbd_initCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) 
{
	int rval;
	if (argc != 2) {
		interp->result = "wrong # of args";
		return TCL_ERROR;
	}
	rval = dbd_init(atoi(argv[1]));
	if(rval == 1)
		return TCL_OK;
	else 
		return TCL_ERROR;
}

int close_sessionCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) 
{
	int rval;
	if (argc != 2) {
		interp->result = "wrong # of args";
		return TCL_ERROR;
	}
	rval = close_session(argv[1]);
	if(rval == 1)
		return TCL_OK;
	else {
		interp->result = "No sessions/databases running";
		return TCL_ERROR;
	}
}

int kill_sessionCmd(ClientData clientData, Tcl_Interp *interp, 
			int argc, char *argv[])
{
	int rval;
	if (argc !=2) {
		interp->result ="wrong # of args";
		return TCL_ERROR;
	}
	rval = kill_session(argv[1]);
	if(rval)
		return TCL_OK;
	else {
		interp->result = "couldn't kill process";
		return TCL_ERROR;
	}
}

int session_infoCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) 
{
 	session_info();
	return TCL_OK;
}

int restartCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[])
{
	char buf[512];
	FILE *setup;
	int code;

	delete_db("pas");
	delete_db("path");
	delete_db("radis");
	if(argc == 1)
		if((setup=fopen("q.setup", "r"))==NULL)
			perror("restart");
	else if (argc == 2)
		if((setup=fopen(argv[1], "r"))==NULL)
			perror("restart");
	else {
		interp->result = "wrong # of args";
		return TCL_ERROR;
	}
	while(fgets(buf, sizeof buf, setup)) {
		code = Tcl_GlobalEval(interp, buf);	
	}
	return TCL_OK;
}

int delete_dbCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[])
{
	int rval; 

	if(argc != 2)
		interp->result = "wrong # of args";
	rval=delete_db(argv[1]);
	return rval?TCL_OK:TCL_ERROR;
}
int dbd_tcl_init(Tcl_Interp *interp)
{
	Tcl_DeleteCommand(interp, "exit");
	Tcl_CreateCommand(interp, "open_session", open_sessionCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "close_session", close_sessionCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "new_db", new_dbCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "qserv_init", dbd_initCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "session_info", session_infoCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "kill_session", kill_sessionCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "q_restart", restartCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateCommand(interp, "delete_db", delete_dbCmd,
			(ClientData *) NULL, (Tcl_CmdDeleteProc *) NULL);
}
