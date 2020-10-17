#include <stdio.h>
#include "myddb.h"

int main (void) {
	/*
		gcc -o myddb_ex.exe myddb_ex.c myddb.c -s -Wall
	*/
	printf("\n MyDDB C\n");

	myddb_init();
	myddb("PATH ../data FROM helloworld PRINT 1");
	myddb_close();

	return 0;
}
