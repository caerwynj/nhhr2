/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* interface to pid table for the qserver */
#ifndef PID_TABLE_H
#define PID_TABLE_H

int add_pid_entry(char * dbname, unsigned pid);

int kill1_pid(char * dbname);

int kill_all_pid(char * dbname);

unsigned int get_pid(char * dbname);

void print_pid_table(void);

int get_n_pid_entries(char * dbname);

#endif
