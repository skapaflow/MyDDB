/*
Copyright (c) 2020 Alberto Ferreira junior

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

/* My Domestic Data Bank [v0.1] 2020
 _________________                _______      _______      _________
/\   ____   ____  \   ___    ___ /\   ___\___ /\   ___\___ /\   ___  \
\ \  \__/\  \__/\  \ /\  \__/\  \\ \  \__/\  \\ \  \__/\  \\ \  \_/\ /_
 \ \  \ \ \  \ \ \  \\ \_______  \\ \  \ \ \  \\ \  \ \ \  \\ \   ___  \
  \ \  \ \ \  \ \ \  \\/_______\  \\ \  \_\/  / \ \  \_\/  / \ \  \_/\  \
   \ \__\ \ \__\ \ \__\ /\________/ \ \______/   \ \______/   \ \_______/
    \/__/  \/__/  \/__/ \/_______/   \/_____/     \/_____/     \/______/
*/

#ifndef MYDDB_H
#define MYDDB_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
	#include <windows.h>
	#include <conio.h>
	#define WIN_OS
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DDBMIN 128
#define DDBMID 256
#define DDBMAX 1024

#define ETX 3       /* end of text            */
#define _ETX "\003" /* (oct) end of text      */
#define GS 29       /* groupe separator       */
#define _GS "\035"  /* (oct) groupe separator */

#define MYDDB_DOT_TYPE ".ddb"

#define E00 "\n [!] myddb_error:%d: command \"%s\" invalid\n"
#define E01 "\n [!] myddb_error:%d: command \"%s\" without antecedent WHERE\n"
#define E02 "\n [!] myddb_error:%d: command \"%s\" without antecedent FROM\n"
#define E03 "\n [!] myddb_error:%d: command \"%s\" without arguments\n"
#define E04 "\n [!] myddb_error:%d: command \"%s\" without sinal de atribuição em seu argumento\n"

#define W00 "\n [!] myddb_warning:%d: directory \"%s\" unexistent\n"
#define W01 "\n [!] myddb_warning:%d: argument \"%s\" repeated\n"
#define W02 "\n [!] myddb_warning:%d: argument \"%s\" to many\n"
#define W03 "\n [!] myddb_warning:%d: column \"%s\" unexistent\n"
#define W04 "\n [!] myddb_warning:%d: column \"%s\" existent\n"
#define W05 "\n [!] myddb_warning:%d: line \"%s\" unexistent\n"
#define W06 "\n [!] myddb_warning:%d: file \"%s\" unexistent\n"
#define W07 "\n [!] myddb_warning:%d: file \"%s\" exclude\n"
#define W08 "\n [!] myddb_warning:%d: file \"%s\" empty\n"
#define W09 "\n [!] myddb_warning:%d: file \"%s\" emptyness\n"
#define W10 "\n [!] myddb_warning:%d: file \"%s\" corrupted\n"
#define W11 "\n [!] myddb_warning:%d: file \"%s\" created\n"
#define W12 "\n [!] myddb_warning:%d: file \"%s\" can't be modify\n"
#define W13 "\n [!] myddb_warning:%d: file \"%s\" can't created or exist\n"
#define W14 "\n [!] myddb_warning:%d: \"%d\" column insert in empty table\n"
#define W15 "\n [!] myddb_warning:%d: column \"%s\" insert in table\n"
#define W16 "\n [!] myddb_warning:%d: column \"%s\" repeated\n"

#define RUNSIG return EXIT_SUCCESS /* 0 */
#define RINT   return EXIT_FAILURE /* 1 */
#define RSIG   return -1
#define RVOID  return
#define RBREAK break
#define RNULL

#define BYTES          999	/* 1 byte - byte     */
#define KB          999999	/* 2 byte - Kilobyte */
#define MB       999999999	/* 3 byte - megabyte */
#define GB    999999999999	/* 4 byte - gigabyte */
#define TB 999999999999999	/* 5 byte - terabyte */

#ifdef WIN_OS
	#define myddb_ftell _ftelli64
#else
	#define myddb_ftell ftello
#endif

#define error(...) printf("\t:%d:\n", __LINE__)
#define stop(...) printf("\t:%d:\n", __LINE__), getch()
#define warning(a,b,_return) {printf(a,__LINE__,b); _return;}

#define isutf8(c) (((c)&0xc0)==0x80)
#define coor(x,y) (int) (abs(x) + (abs(y) * (t->w)))
#define gfret(a,b) (a < b ? GS : ETX) /* guitar fret */

#define myddb_set(c,v,i,n) memset((c*)v, i, sizeof(c)*n)
#define myddb_free(f) if (f != NULL) free(f), f = NULL
#define myddb_fclose(f) if (f != NULL) fclose(f), f = NULL

#define myddb_malloc(c,v,n)\
	if ((v = (c*) malloc(sizeof(c) * n)) == NULL) {\
		printf("\n [!] malloc_error:\"%s\":%d\n", __FILE__, __LINE__);\
		getch();\
		exit(EXIT_FAILURE);\
	}\

#define myddb_calloc(c,v,n)\
	if ((v = (c*) calloc(n, sizeof(c))) == NULL) {\
		printf("\n [!] calloc_error:\"%s\":%d\n", __FILE__, __LINE__);\
		getch();\
		exit(EXIT_FAILURE);\
	}\

#define myddb_realloc(c,v,n)\
	if ((v = (c*) realloc(v, sizeof(c) * n)) == NULL) {\
		printf("\n [!] realloc_error:\"%s\":%d\n", __FILE__, __LINE__);\
		getch();\
		exit(EXIT_FAILURE);\
	}\

extern const char *myddb_logo[];
extern const char *commands_list[];
extern FILE *myddb_file;
extern char *myddb_path;
extern char *myddb_name;
extern bool  myddb_fempty; /* verify if file are empty or full */

typedef struct {
	int w;          /* number of columns */
	int h;          /* number of rows */
	int olot;       /* output number */
	int row_size;   /* row string size */
	int col_size;   /* column string size */
	int max_size;   /* max string size per column */
	int col_utf8;   /* number of utf-8 char per column */
	int row_utf8;   /* number of utf-8 char per row */
	int row_match;  /* cross table index */
	char *out;      /* output buffer */
	char *row;      /* row buffer */
	char *tmp;      /* tmp buffer */
	char *column;   /* column buffer */
	char *oselect;  /* column buffer selection */
	char *oinsert;  /* input buffer insertion */
} MYDDBTABLE;

int strlen_utf8 (const char *);
int64_t size_file (FILE *);
const char *strbytes (int64_t);
void fmtsize (char *, int, int64_t);

MYDDBTABLE *get_rows_and_columns (int);
void free_rows_and_columns (MYDDBTABLE *);
bool verify_requested_columns (MYDDBTABLE *, int *);
void get_args (const char *, int);
void free_get_args (void);
void format_command (char *);
void treat_file_path (char *, const char *);
int myddb_cmd_error (char *);

int myddb (const char *, ...);
void myddb_init (void);
void myddb_close (void);
int myddb_open (const char *);
void myddb_show (void);

void myddb_insert (void);
void myddb_update (void);
void myddb_delete (void);
void myddb_drop (void);

void myddb_jump (void);
void myddb_cross (void);
void myddb_column (void);

void myddb_create (const char *);
void myddb_erase (const char *);
void myddb_exclude (const char *);

void myddb_print (int);


#ifdef __cplusplus
}
#endif

#endif /* MYDDB_H */
