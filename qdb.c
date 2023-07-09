/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* qdb.c implementation of the q dtabse module */

#include "qdb.h"
#include "globals.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define TRUE 1
#define FALSE 0

char dbfilename[]="/var/tmp/qdb";
char update_filename[]="/var/tmp/update";

void init_db_file(void)
{
	FILE *qdb;
	
	qdb=fopen(dbfilename,"w");
	fclose(qdb);
}

int get_db_record(char * dbname, queuedb * dbrecord)
{
	FILE *dbfile;
	char db_name[15], sockname[15]; /* fields of queuedb record */
	int socfd, semnums[3], sess_no;
	int record_found=FALSE;
	int i, var;

	if((dbfile=fopen(dbfilename, "r"))==NULL) {
		perror("qdb");
		exit(1);
	}
	do {
	var=fscanf(dbfile, "%s%s%d%d%d%d%d",db_name, sockname, &socfd, &semnums[0], &semnums[1], &semnums[2], &sess_no);
	if(!strcmp(dbname, db_name)) 
		record_found=TRUE;
	} while (!((var==EOF)||record_found));
	fclose(dbfile);
	if(record_found) {
		strcpy(dbrecord->db_name, db_name);
		strcpy(dbrecord->sock_name, sockname);
		dbrecord->socfd = socfd;
		for(i=0;i<3;i++) dbrecord->semnums[i] = semnums[i];
		dbrecord->sess_no = sess_no;
		return 1;
	}else {
	return 0;
	}
}

int put_db_record(queuedb * dbrecord)
{
	FILE *dbfile;
	queuedb db1;
	if((dbfile=fopen(dbfilename,"a+"))==NULL) {
		perror("qdb");
		return 0;
	}

	/* check if dbrecord already exists */
	if (!get_db_record(dbrecord->db_name, &db1))
		fprintf(dbfile, "%s %s %d %d %d %d %d\n",dbrecord->db_name, dbrecord->sock_name, dbrecord->socfd, dbrecord->semnums[0], dbrecord->semnums[1], dbrecord->semnums[2], dbrecord->sess_no);
	else 
		fprintf(stderr, "Record already exists in qdb\n");
	fclose(dbfile);
	return 1;
} 
 
int update_db_record(char * dbname, queuedb * dbrecord)
{
	FILE *dbfile, *update;
	char db_name[15],sockname[15];
	int socfd,semnums[3], sess_no;
	int record_found=FALSE;
	int i, var;
	
	if((update=fopen(update_filename,"w"))==NULL) {
		perror("qdb");
		return 0;
	}

	if((dbfile=fopen(dbfilename,"r"))==NULL) {
		perror("qdb");
		return 0;
	}
	while (fscanf(dbfile, "%s%s%d%d%d%d%d",db_name, sockname, &socfd, &semnums[0], &semnums[1], &semnums[2], &sess_no) != EOF) {
		if(!strcmp(dbname, db_name)) { 
			record_found=TRUE;
			fprintf(update,"%s %s %d %d %d %d %d\n",dbrecord->db_name, dbrecord->sock_name, dbrecord->socfd, dbrecord->semnums[0], dbrecord->semnums[1], dbrecord->semnums[2], dbrecord->sess_no);
		}
		else {
			fprintf(update,"%s %s %d %d %d %d %d\n",db_name, sockname, socfd, semnums[0], semnums[1], semnums[2], sess_no);
		}
	}
	fclose(dbfile);
	fclose(update);
	if(rename(update_filename,dbfilename)){
		perror("renaming file");
		exit(1);
	}
	return record_found?1:0;
}

int remove_db_record(char * dbname)
{
	FILE *dbfile, *update;
	char db_name[15],sockname[15];
	int socfd,semnums[3], sess_no;
	int record_found=FALSE;
	int i, var;
	
	if((update=fopen(update_filename,"w"))==NULL) {
		perror("qdb");
		return 0;
	}
	if((dbfile=fopen(dbfilename,"r"))==NULL) {
		perror("qdb");
		return 0;
	}
	while (fscanf(dbfile, "%s%s%d%d%d%d%d",db_name, sockname, &socfd, &semnums[0], &semnums[1], &semnums[2], &sess_no)  != EOF) {
		if(!strcmp(dbname, db_name)) 
			record_found=TRUE;
		else {
			fprintf(update, "%s %s %d %d %d %d %d\n",db_name, sockname, socfd, semnums[0], semnums[1], semnums[2], sess_no);
		}
	} 
	fclose(dbfile);
	fclose(update);
	if(rename(update_filename, dbfilename)){
		perror("renaming file");
		exit(1);
	}
	return record_found?1:0;
}

void get_all_semnums(int * sems, int *num)
{
	queuedb db;
	FILE * dbfile;
	int i,j,count=0;
	
	if((dbfile=fopen(dbfilename,"r"))==NULL) {
		perror("opening file");
		exit(1);
	}

	j=0;
	while(fscanf(dbfile,"%s%s%d%d%d%d%d",db.db_name, db.sock_name, &db.socfd, &db.semnums[0], &db.semnums[1], &db.semnums[2], &db.sess_no) !=EOF) {
		for(i=0;i<3;i++) {
			sems[i+j]=db.semnums[i];
			count++;
		}
		j +=3;
	}
	*num=count;
	fclose(dbfile);
}

int get_free_sems(int sems3[])
{
	/* db_init() must have been called before this func */

	int num, used[maxsems], sems[maxsems]; /* instead of 10 put maxsems */
	int i, freesem=0;

	for(i=0;i<maxsems;i++) used[i]=FALSE;
	get_all_semnums(sems, &num);
	if(num==maxsems) 
		return 0;
	for(i=0;i<num;i++)
		used[sems[i]]=TRUE;
	for(i=1;i<maxsems;i++) {
		if(used[i]==FALSE) {
			freesem=i;
			break;
		}
	}
	for(i=0;i<3;i++)
		sems3[i]=freesem+i;
	return freesem;
}

int get_semid()
{
	key_t key;
	int my_semid;
	union semun mysemun;

	if ((key = ftok("/home/cjones/diary", 'z'))<0) {
		perror("get_semid");
	}
	
	if ((my_semid = semget(key, 0,0666))<0) {
		perror("get_semid");	
	}
	
	return my_semid;
}
