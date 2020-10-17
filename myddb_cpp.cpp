#include <iostream>
#include "myddb.h"

using namespace std;

int main (void) {
	/*
		g++ -o myddb_cpp.exe myddb_cpp.cpp myddb.c -s -Wall
	*/
	cout << "\n MyDDB CPP!\n";

	myddb_init();
	myddb("PATH data FROM helloworld PRINT 1");
	myddb_close();

	return 0;
}