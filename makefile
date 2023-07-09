# (c) 1996 Nevill Hall and District Trust. I.T. Department.
# Author: Caerwyn Jones
# Last modified: 12/08/96

# Makefile for the complete placement project.
# make all builds the whole qserver and subsystems
SHELL=/bin/sh

all: 	db sessions qtclsh Qserver qsh 

install: db sessions Qserver qsh
	cp db pas path Qserver qsh /home/cjones/bin

Qserver: Qserver.o tcl_qserv.o dbd.o pid_table.o qdb.o
	cc -o Qserver Qserver.o tcl_qserv.o dbd.o pid_table.o qdb.o -ltcl -lm

Qserver.o: Qserver.c
	cc -c Qserver.c

qsh: qsh.c
	cc -o qsh qsh.c

qtclsh:  tcl_qserv.o dbd.o qdb.o pid_table.o qtclsh.o
	cc -o qtclsh tcl_qserv.o dbd.o qdb.o pid_table.o qtclsh.o -ltcl -lm

qtclsh.o: qtclsh.c
	cc -c qtclsh.c

tcl_qserv.o: tcl_qserv.c 
	cc -c tcl_qserv.c

db:	qdb.o db.o	
	cc -o db db.o qdb.o 

sessions: pas path

db.o:	db.c
	cc -c db.c

qdb.o:	qdb.c qdb.h
	cc -c qdb.c

pid_table.o: pid_table.c pid_table.h
	cc -c pid_table.c

dbd.o: 	dbd.c dbd.h 
	cc -c dbd.c

pas: pas.o qdb.o 
	cc -o pas pas.o qdb.o

path: path.o qdb.o
	cc -o path path.o qdb.o

pas.o: pas.c
	cc -c pas.c
path.o: path.c
	cc -c path.c

cleansocks:
	rm -f /var/tmp/pasv /var/tmp/patv /var/tmp/radv /var/tmp/rend
clean:
	rm -f core a.out pid_table.o db.o dbd.o qdb.o Qserver.o pas.o \
		path.o 

