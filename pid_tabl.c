/* (c) 1996 Nevill Hall and District Trust. I.T. Department.
 * Author: Caerwyn Jones
 * Last modified: 12/08/96
*/

/* A linked list of process IDs and database names */
#include "pid_table.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef struct pnode{
	struct pnode *next;
	char dbname[10];
	unsigned int pid;
}Pidnode;

Pidnode *pidtable=0;

int add_pid_entry(char * dbname, unsigned int pid) 
{
	Pidnode *node, *nextnode;

	node = (Pidnode *) malloc(sizeof(Pidnode));
	if (node == NULL) {
		perror("bugger, no memory");
		return 0;
	} else {
		strcpy(node->dbname, dbname);
		node->pid = pid;
		node->next = NULL;
	}

	if(pidtable == NULL)
		pidtable = node;  /* assign to first node of list */
	else {   		  /* join on to the end of list */
		nextnode=pidtable;
		while (nextnode->next != NULL) {
			nextnode=nextnode->next;
		}
		nextnode->next=node;
	}
	return 1;
}
	

int kill1_pid(char * dbname)
{
	Pidnode * node, *prev;

	prev = node = pidtable;
	while(node) { 
		if(strcmp(dbname, node->dbname) == 0) {
			if(kill(node->pid, SIGKILL)<0) 	
				perror("killing 1 process");
			if (node == pidtable) 
				pidtable=node->next;
			else
				prev->next = node->next;
			free(node);
			return 1;
		}
		prev = node;
		node=node->next;
	}
	return 0;
}

int kill_all_pid(char * dbname)
{
	Pidnode * node, *prev;
	int killed=FALSE, count=0;

	prev = node = pidtable;
	while(node) { 
		if(!strcmp(dbname, node->dbname)) {
			if(kill(node->pid,SIGKILL)<0)
				perror("killing process");
			if (node == pidtable)
				pidtable = node->next;
			else 
				prev->next = node->next;
			free(node);
			killed=TRUE;
			count++;
		}
		prev = node;
		node=node->next;
	}
	return killed?count:0;
}

unsigned int get_pid(char * dbname)
{
	Pidnode *node;

	node = pidtable;
	while(node != NULL) {
		if(!strcmp(node->dbname, dbname)) {
			return node->pid;
		}
		node = node->next;
	}
	return 0; /* dbname not found */
}

void print_pid_table(void)
{
	Pidnode *node;
	
	node = pidtable;
	while(node) {
		fprintf(stderr,"dbname:%s  pid:%d\n",node->dbname, node->pid);
		node=node->next;
	}
}

int get_n_pid_entries(char * dbname)
{
	int count=0;
	Pidnode *node;
	
	node=pidtable;
	while(node) {
		if(!strcmp(dbname, node->dbname)) 
			count++;
		node=node->next;
	}
	return count;
}

int remove_pid_entry(unsigned int pid)
{
	Pidnode * node, *prev;

	prev = node = pidtable;
	while(node) { 
		if(pid == node->pid) {
			if (node == pidtable) 
				pidtable=node->next;
			else
				prev->next = node->next;
			free(node);
			return 1;
		}
		prev = node;
		node=node->next;
	}
	return 0;
}

