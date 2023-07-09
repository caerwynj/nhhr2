/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* the db server interface - handle the opening/closing of new sessions
   the adding/removing of databases and return info on the queue
*/
#ifndef DBD_H
#define DBD_H

#include "qdb.h"
#include <stdio.h>


int open_session(int argc, char *argv[]);
int close_session(char * dbname);
int kill_session(char * dbname);
int kill_pid(unsigned pid);
int dbd_init();
int new_db(char * dbname);
int close_db(char * dbname);
int delete_db(char * dbname);
void session_info(void);

#endif
