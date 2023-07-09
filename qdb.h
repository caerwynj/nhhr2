/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* qdb - the database queue module types for access by the queue subsystem. */
#ifndef QDB_H
#define QDB_H

#define MUTEX 0
#define QUEUE 1
#define MAX   2
#define FILELOCK 0 

typedef struct quedb {
	char db_name[15];
	char sock_name[15];
	int socfd;
	int semnums[3];
	int sess_no;
}queuedb;

int semid;
int maxsems;
extern char *semid_pathname;
extern char semid_proj;

/* Interface to qdb - fuction prototypes */

void init_db_file(void);

int get_db_record(char * dbname, queuedb * dbrecord);

int update_db_record(char * dbname, queuedb * dbrecord);

int remove_db_record(char * dbname);

int put_db_record(queuedb * dbrecord);

void get_all_semnums(int *sems, int *num);

int get_free_sems(int sems3[]);

int get_semid();

#endif
