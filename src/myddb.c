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

#include "myddb.h"

const char myddb_logo[] =
	"\n MYD      BMY              BMYDDBMYD    YDDBMYDDB    DBMYDDBMYDD\n"
	" MYDD    DBMY DDB      YDD BMYDDBMYDDB  YDDBMYDDBMY  DBMYDDBMYDDB\n"
	" MYDDB  DDBMY  DBM    MYD  BMY     DDBM YDD     BMYD DBM     YDDB\n"
	" MYDDBMYDDBMY   BMY  BMY   BMY      DBM YDD      MYD DBMYDDBMYDD\n"
	" MYD BMYD BMY    MYDDBM    BMY      DBM YDD      MYD DBMYDDBMYDD\n"
	" MYD  MY  BMY     YDDB     BMY     DDBM YDD     BMYD DBM     YDDB\n"
	" MYD      BMY    MYDD      BMYDDBMYDDB  YDDBMYDDBMY  DBMYDDBMYDDB\n"
	" MYD      BMY DDBMY        BMYDDBMYD    YDDBMYDDB    DBMYDDBMYDD\n\n"
	" My Domestic Data Bank [v1.0 - 2020]\n (Type \"Help\" for more information)\n";

const char commands_list[] =
	"\n +------------------------------------------------------+\n"
	" | COMMAND  EX                   DEFINITION             |\n"
	" +------------------------------------------------------+\n"
	" | PATH     /user/desktop        define database path   |\n"
	" | FROM     from <file>          define table           |\n"
	" | ADD      name = Alberto       add row                |\n"
	" | WHERE    name = Alberto       set column and row     |\n"
	" | UPDATE   name = Alberto       update row             |\n"
	" | DEL      name = Alberto       delete row             |\n"
	" | CROSS    (id = 0 name = 1)    cross column and row   |\n"
	" | COL      (id cmp str = name)  add colums and update  |\n"
	" | DROP     (id name date)       drop colums            |\n"
	" | JUMP     (name = 0 id = 2)    order colums           |\n"
	" | CREATE   create <file>        create table           |\n"
	" | ERASE    erase <file>         erase records of table |\n"
	" | EXCLUDE  exclude <file>       exclude table          |\n"
	" | PRINT    0, 1, 2              show tabela format     |\n"
	" | SHOW     show                 show database          |\n"
	" | CLEAR    clear                clear screen           |\n"
	" | VER      ver                  show MyDDB version     |\n"
	" | CLOSE    or type dot (.)      close MyDDB            |\n"
	" +------------------------------------------------------+\n";

#if !NIX_OS
	int output_state_shell;
#endif

int strlen_utf8 (const char *s) {

	int i, j;
	for (i = j = 0; s[i]; i++)
		if ((s[i] & 0xc0) != 0x80)
			j++;
	return j;
}

#if !NIX_OS
char *strupr (char *s) {

	for (char *p = s; *p; p++)
		*p = toupper(*p);
	return s;
}
#endif

int64_t size_file (FILE *f) {

	fseek(f, 0, SEEK_END);
	int64_t pos = myddb_ftell(f);
	rewind(f);
	return pos;
}

const char *strbytes (int64_t v) {

	const char *b[] = {"B ","KB","MB","GB","TB",NULL};
	if      (v >= 0     && v <= BYTES) return b[0];
	else if (v >= BYTES && v <=    KB) return b[1];
	else if (v >=    KB && v <=    MB) return b[2];
	else if (v >=    MB && v <=    GB) return b[3];
	else return b[4];
	return NULL;
}

void fmtsize (char *buf, int len, int64_t value) {

	myddb_memset(char, buf, 0, len);
	char str[18];
	snprintf(str, 18, (!NIX_OS?"%I64d":"%ld"), value);
	for (int i = 16, j = 0, k = strlen(str + 1); i >= 0; i--) {
		buf[i] = ((j++ < 3) && (k >= 0) ? str[k--] : '.');
		j = (buf[i] == '.' ? 0 : j);
	}
}

MYDDBTABLE *get_rows_and_columns (int channel) {

	int i = 0;
	int j = 0;
	int k = 0;
	int h_len = 1;
	int reach_line = DDBMIN;
	int reach_table = DDBMIN;
	uint8_t c = 0;
	MYDDBTABLE *t = NULL;

	myddb_calloc(MYDDBTABLE, t, DDBMIN);
	myddb_calloc(char, t->column, DDBMIN);

	/* limit channel */
	channel = ((channel == 0 || channel > 2 || channel < 0) ? 1 : channel);

	if (channel > 1)
		myddb_calloc(char, t->row, DDBMIN);

	rewind(myddb_file);

	/* set row_utf8 == 0 */
	t->row_utf8 = 0;

	/* read column */
	while (true) {

		c = fgetc(myddb_file);

		if (feof(myddb_file))
			break;

		if (c == GS || c == ETX) {
			/*{{{ ETX*/
			/* group separator */
			t[i].column[j] = 0;
			t[i].col_size = strlen_utf8(t[i].column);
			t[i].max_size = t[i].col_size;
			/* get max size in the line */
			if (t[i].col_size > t->max_line)
				t->max_line = t[i].col_size;
			t->w += 1;
			/*}}} ETX*/
			/* end of line */
			if (c == ETX)
				break;
			i++;
			j = 0;
			/* more memory for table */
			if (i >= (reach_table - 1))
				myddb_realloc(MYDDBTABLE, t, (reach_table += DDBMIN));
			myddb_calloc(char, t[i].column, DDBMIN);
		} else {
			/* more memory for column */
			if (j >= (reach_line - 1))
				myddb_realloc(char, t[i].column, (reach_line += DDBMIN));
			t[i].col_utf8 += isutf8(c);
			t[i].column[j++] = c;
		}
	}

#if 0
	printf("\n");
	for (int l = 0; l < t->w; l++)
		printf("%-*s|", (t[l].col_size + t[l].col_utf8), t[l].column);
	printf("\n");
#endif

	i = 0;
	j = 0;
	reach_line = DDBMIN;
	reach_table = DDBMIN;
	/* set row_utf8 == 0 */
	t->row_utf8 = 0;

	/* read rows */
	while (channel == 2) {

		c = fgetc(myddb_file);

		if (feof(myddb_file))
			break;

		if (c == ETX) {
			t->h += 1;
			/* get error */
			if ((t->w - 1) != k) {
				free_rows_and_columns(t);
				warning(W10, myddb_name, return NULL);
			}
		}

		if (c == GS || c == ETX) {
			t[i].row[j] = 0;
			t[i].row_size = strlen_utf8(t[i].row);
			k = (k == t->w ? 0 : k);
			/* get max row size */
			if (t[i].row_size > t[k].max_size)
				t[k].max_size = t[i].row_size;
			/* get max size in the line */
			if (t[i].row_size > t[h_len].max_line)
				t[h_len].max_line = t[i].row_size;
			// printf(" %d:%.2d [%s]\n", h_len, t[h_len].max_line, t[i].row);
			h_len = (t->h + 1);
			k++;
			i++;
			/* reset t[i].row_utf8 */
			t[i].row_utf8 = 0;
			j = 0;
			/* more memory for table */
			if (i >= (reach_table - 1))
				myddb_realloc(MYDDBTABLE, t, (reach_table += DDBMIN));
			myddb_calloc(char, t[i].row, DDBMIN);
		} else {
			/* more memory for row */
			if (j >= (reach_line - 1))
				myddb_realloc(char, t[i].row, (reach_line += DDBMIN));
			t[i].row_utf8 += isutf8(c);
			t[i].row[j++] = c;
		}
	}

#if 0
	printf("\n");
	for (int y = 0; y < t->h; y++) {
		for (int x = 0; x < t->w; x++)
			printf("%-*s|", t[x].max_size, t[coor(x,y)].row);
		printf("\n");
	}
	printf("\n");
#endif

	rewind(myddb_file);

	return t;
}

void free_rows_and_columns (MYDDBTABLE *t) {

	if (t != NULL) {
		if (t->column)
			for (int i = 0; i < t->w; i++)
				myddb_free(t[i].column);
		if (t->row)
			for (int i = 0; i < (t->w * t->h); i++)
				myddb_free(t[i].row);
		myddb_free(t);
	}
}

bool verify_requested_columns (MYDDBTABLE *t, int *error) {

	for (int i = 0; i < omyddb->olot; i++)
		for (int j = 0; j <= t->w; j++) {
			if (j == t->w) {
				*error = i;
				return true; /* error */
			} else if (!strcmp(t[j].column, omyddb[i].oselect))
				break;
		}
	*error = 0;
	return false; /* OK */
}

void get_args (const char *str, int channel) {

	/*
	 * get_args("str", 1) get only columns
	 * get_args("str", 2) get columns and rows
	 * get_args("str", 3) get columns or rows
	 */

	int i = 0;
	int reach = DDBMIN;
	char *buf = NULL;
	myddb_malloc(char, buf, DDBMID);

	if (omyddb == NULL) {
		myddb_calloc(MYDDBTABLE, omyddb, DDBMIN);
		omyddb->olot = 0;
	}

	/* split args */
	char *arr = strtok(strcpy(buf, str), "\003");
	do {
		bool assignment = !!strchr(arr, '=');
		/* get column */
		if (channel == 1) {
			omyddb[i].oselect = strdup(arr);
			if (assignment)
				warning(E06, arr, RBREAK);
		/* get column and row */
		} else if (channel == 2 || channel == 3) {
			if (assignment) {
				char *tmp = strdup(arr);
				char *t = tmp;
				while (*t && *t++ != '=');
				*--t = 0;
				t++;
				omyddb[i].oselect = strdup(tmp);
				/* substitute GS for 'space(32)' */
				for (char *b = t; *b; b++)
					*b = (*b == GS ? ' ' : *b);
				omyddb[i].oinsert = strdup(t);
				myddb_free(tmp);
			} else if (channel == 3) {
				omyddb[i].oselect = strdup(arr);
				omyddb[i].oinsert = strdup(_ETX);
			} else
				warning(E05, arr, RBREAK);
		}
		i++;
		/* add more memory */
		if (i >= (reach - 1))
			myddb_realloc(MYDDBTABLE, omyddb, (reach += DDBMIN));
	} while ((arr = strtok(NULL, "\003")) != NULL);

	omyddb->olot = i;
	free(buf);

#if 0
	printf("\n ");
	for (int j = 0; j < omyddb->olot; j++)
		printf("%s=%s|", omyddb[j].oselect, omyddb[j].oinsert);
	printf("\n");
#endif
}

void free_get_args (void) {

	if (omyddb != NULL) {
		for (int i = 0; i < omyddb->olot; i++) {
			myddb_free(omyddb[i].oselect);
			myddb_free(omyddb[i].oinsert);
			myddb_free(omyddb[i].out);
		}
		myddb_free(omyddb);
	}
}

void format_command (char *cmd) {

	if (!*cmd)
		return;

	char *buf = strdup(cmd);
	char *a = buf;
	char *b = buf;
	char *c = buf;

	/* strdup: get internal error from malloc into strdup */
	if (buf == NULL) {
		printf("\n [!] strdup_error:\"%s\":%d\n", __FILE__, __LINE__);
		return; 
	}

	/* treat quotes */
	do {
		if (*b == '\'' || *b == '\"') {
			char s = *b;
			while (*++b != s)
				*b = (*b == ' ' ? GS : *b);
		}
	} while (*b++);

	a = (b = (c = buf));

	/* normalize space */
	do {
		if (*a++ != ' ' || *a != ' ')
			*b++ = *c;
	} while (*c++);
	(*b == ' ' ? b++ : b);

	b = (c = buf);

	/* handle assignment */
	do {
		if (!(*b == ' ' && (*(b-1) == '=' || *(b+1) == '=')))
			*c++ = *b;
	} while (*b++);

	b = buf;

	/* separate sintax */
	do {
		if (*b == '(')
			while (*++b != ')') {
				if (*b == '\'' || *b == '\"') { /* jump quote */
					char s = *b++;
					while (*b != s)
						b++;
				}
				*b = (*b == ' ' ? ETX : *b);
			}
	} while (*b++);

	b = (c = buf);

	/* remove parentheses and quotes */
	do {
		if (*b == '\'' || *b == '\"') { /* jump quotes */
			char s = *b++;
			while (*b != s)
				*c++ = *b++;
			*c = *b++;
		}
		if (*b != '(' && *b != ')')
			*c++ = *b;
	} while (*b++);

	b = (c = buf);

	strcpy(cmd, buf);
	free(buf);
}

void treat_file_path (char *des, const char *file) {

	/*  concatenate '.ddb' in the end */
	if (*myddb_path && !strstr(file, MYDDB_DOT_TYPE))
		sprintf(des, "%s" OS_BAR "%s%s", myddb_path, file, MYDDB_DOT_TYPE);
}

int myddb_cmd_error (char *cmd) {

	if (!*myddb_name)
		warning(E02, cmd, RINT);
	if (!*cmd)
		warning(E03, cmd, RINT);
	if (!myddb_fempty)
		warning(W08, myddb_name, RINT);
	if (!strstr(cmd, "="))
		warning(E04, cmd, RINT);
	return false;
}

int myddb (const char *str, ...) {

	/* free cross memory */
	free_get_args();

	const char *myddb_cmd[] = {
		"PATH","FROM","ADD","WHERE",        /* ...3  */
		"UPDATE","DEL","CROSS","PRINT",     /* ...7  */
		"HELP", "COL","DROP","JUMP",        /* ...11 */
		"SHOW","CREATE","ERASE", "EXCLUDE", /* ...15 */
		"CLEAR","VER","CLOSE",".",          /* ...18 */
		NULL
	};

	int cmditer = 0;
	int return_exit = EXIT_FAILURE;
	char *buf = NULL;
	char **cmdline = NULL;

	/* resolve print arg */
	myddb_calloc(char, buf, DDBMID);
	va_list ap;
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);

	if (!*buf)
		goto myddb_exit;

	/* format input argument */
	format_command(buf);

	/* decompose commands */
	myddb_malloc(char *, cmdline, DDBMIN);
	char *aux = strtok(buf, " ");
	do {
		cmdline[cmditer++] = strdup(aux);
	} while ((aux = strtok(NULL, " ")) != NULL);

	/* select commands */
	for (int i = 0, j = 0; i < cmditer; i++) {
		/* get next command */
		for (j = 0; myddb_cmd[j] != NULL && (strcmp(strupr(cmdline[i]), myddb_cmd[j])); j++);
	#if 0
		printf("\r command{ %s }", cmdline[i]);
	#endif
		switch (j) {
			case 0: /*############### PATH ###############*/
				if (!*cmdline[i+1] || cmditer == 1)
					printf("\n Databank directory: \"%s\"\n", myddb_path);
				else {
					strcpy(myddb_path, cmdline[i+1]);
					DIR *dir = NULL;
					/* verify directory */
					if ((dir = opendir(myddb_path)))
						closedir(dir);
					else {
						/* set default directory */
						strcpy(myddb_path, ".");
						warning(W00, cmdline[i+1], RBREAK);
					}
				}
				break;
			case 1: /*############### FROM ###############*/
				if (!*cmdline[i+1] || cmditer == 1)
					warning(E03, cmdline[i], RBREAK);
				if (myddb_open(cmdline[i+1]))
					goto myddb_exit;
				break;
			case 2: /*############### ADD ###############*/
				if (myddb_cmd_error(cmdline[i+1]))
					break;
				get_args(cmdline[i+1], 2);
				myddb_insert();
				free_get_args();
				break;
			case 3: /*############### WHERE ###############*/
				if (!*myddb_name)
					warning(E02, cmdline[i], RBREAK);
				get_args(cmdline[i+1], 2);
				myddb_cross();
				break;
			case 4: /*############### UPDATE ###############*/
				if (strcmp(myddb_cmd[3], cmdline[2]))
					warning(E01, cmdline[i], RBREAK);
				get_args(cmdline[i+1], 2);
				myddb_update();
				free_get_args();
				break;
			case 5: /*############### DEL ###############*/
				get_args(cmdline[i+1], 2);
				myddb_delete();
				free_get_args();
				break;
			case 6:/*############### CROSS ###############*/
				get_args(cmdline[i+1], 3);
				myddb_cross();
				break;
			case 7: /*############### PRINT ###############*/
				if (!*myddb_name)
					warning(E02, cmdline[i], RBREAK);
				if (!myddb_fempty)
					warning(W08, myddb_name, RBREAK);
				myddb_print(atoi(cmdline[i+1]));
				break;
			case 8: /*############### HELP ###############*/
				printf(commands_list);
				i--; /* no argument */
				break;
			case 9: /*############### COL ###############*/
				if (!*myddb_name)
					warning(E02, cmdline[i], RBREAK);
				if (!*cmdline[i+1])
					warning(E03, cmdline[i], RBREAK);
				get_args(cmdline[i+1], 3);
				myddb_column();
				free_get_args();
				break;
			case 10: /*############### DROP ###############*/
				if (!*myddb_name)
					warning(E02, cmdline[i], RBREAK);
				if (!*cmdline[i+1])
					warning(E03, cmdline[i], RBREAK);
				if (!myddb_fempty)
					warning(W08, myddb_name, RBREAK);
				get_args(cmdline[i+1], 1);
				myddb_drop();
				free_get_args();
				break;
			case 11: /*############### JUMP ###############*/
				if (myddb_cmd_error(cmdline[i+1]))
					break;
				get_args(cmdline[i+1], 2);
				myddb_jump();
				free_get_args();
				break;
			case 12: /*############### SHOW ###############*/
				myddb_show();
				i--; /* no argument */
				break;
			case 13: /*############### CREATE ###############*/
				if (!*cmdline[i+1])
					warning(E03, cmdline[i], RBREAK);
				myddb_create(cmdline[i+1]);
				break;
			case 14: /*############### ERASE ###############*/
				if (!*cmdline[i+1])
					warning(E03, cmdline[i], RBREAK);
				myddb_erase(cmdline[i+1]);
				break;
			case 15: /*############### EXCLUDE ###############*/
				if (!*cmdline[i+1])
					warning(E03, cmdline[i], RBREAK);
				myddb_exclude(cmdline[i+1]);
				break;
			case 16: /*############### CLEAR ###############*/
				system(!NIX_OS ? "cls" : "clear");
				break;
				/*############### VER ###############*/
			case 17:
				printf(myddb_logo);
				i--; /* no argument */
				break;
				/*############### CLOSE ###############*/
			case 18:
			case 19:
				return_exit = EXIT_SUCCESS;
				myddb_fclose(myddb_file);
				break;
				/*############### ERRO ###############*/
			default: warning(E00, cmdline[i], goto myddb_exit); break;
		}
		/* jump argument */
		i++;
	}

myddb_exit:

	myddb_free(buf);

	for (int i = 0; i < cmditer; i++)
		myddb_free(cmdline[i]);
	myddb_free(cmdline);

	return return_exit;
}

void myddb_init (void) {

	/* get console state */
#if !NIX_OS
	output_state_shell = GetConsoleOutputCP();
#endif

	omyddb = NULL;
	myddb_file = NULL;
	myddb_path = NULL;
	myddb_name = NULL;
	myddb_fempty = false; /* verify if file are empty or full */

	myddb_calloc(char, myddb_path, DDBMAX);
	myddb_calloc(char, myddb_name, DDBMAX);

	/* set default directory */
	*myddb_path = '.';
}

void myddb_close (void) {

	myddb_fempty = false;

	myddb_free(omyddb);
	myddb_free(myddb_path);
	myddb_free(myddb_name);
	myddb_fclose(myddb_file);

	free_get_args();
}

int myddb_open (const char *file) {

	DIR *dir = NULL;

	/* verify directory */
	if (!(dir = opendir(myddb_path))) {
		strcpy(myddb_path, ".");
		myddb_memset(char, myddb_name, 0, DDBMAX);
		warning(W00, myddb_path, RINT);
	}
	closedir(dir);

	/* verify format file ".ddb" */
	treat_file_path(myddb_name, file);

	/* open table from database */
	if (myddb_file != NULL)
		myddb_fclose(myddb_file);
	if (!(myddb_file = fopen(myddb_name, "r+"))) {
		warning(W06, myddb_name, RINT);
		myddb_memset(char, myddb_name, 0, DDBMAX);
		return true;
	}

	/* verify file */
	rewind(myddb_file);
	myddb_fempty = (fgetc(myddb_file) >= 0) ? true : false;

	return false;
}

void myddb_show (void) {

	FILE *file;
	DIR *dir = NULL;
	int64_t msize;
	struct dirent *entry;

	char *buf = NULL;
	char *path = NULL;
	myddb_calloc(char, buf, 80);
	myddb_calloc(char, path, DDBMIN);

	/* getcwd - get the pathname of the current working directory */
	getcwd(path, DDBMIN);
	printf("\n DIR %s" OS_BAR "%s\n\n", path, myddb_path);

	/* verify directory */
	if (!(dir = opendir(myddb_path)))
		warning(W00, myddb_path, goto show_exit);

	/* get files */
	while ((entry = readdir(dir))) {
		snprintf(path, DDBMAX, "%s" OS_BAR "%s", myddb_path, entry->d_name);
		if (strstr(entry->d_name, MYDDB_DOT_TYPE)) {
			if ((file = fopen(path, "r"))) {
				msize = size_file(file);
				fmtsize(buf, 80, msize);
				printf(" %s %s   %s\n", buf, strbytes(msize), entry->d_name);
				fclose(file);
			}
		}
	}

show_exit:

	if (!*buf)
		printf(" [!] empty\n");

	if (dir != NULL)
		closedir(dir);

	myddb_free(buf);
	myddb_free(path);
}

void myddb_insert (void) {

	int *v = NULL;
	int error;
	MYDDBTABLE *t = NULL;

	if ((t = get_rows_and_columns(1)) != NULL) {

		myddb_calloc(int, v, (t->w + 1));

		/* verify columns */
		if (verify_requested_columns(t, &error))
			warning(W03, omyddb[error].oselect, goto insert_exit);

		/* find column order */
		for (int i = 0; i < omyddb->olot; i++)
			for (int j = 0; j < t->w; j++)
				if (!strcmp(t[j].column, omyddb[i].oselect)) {
					v[i] = j;
					break;
				}

		/* write row */
		fseek(myddb_file, size_file(myddb_file), SEEK_SET);
		for (int i = 0; i < t->w; i++) {
			for (int j = 0; j < omyddb->olot; j++)
				if (v[j] == i) {
					fprintf(myddb_file, "%s", omyddb[j].oinsert);
					break;
				}
			fprintf(myddb_file, "%c", gfret(i, (t->w - 1)));
		}

	insert_exit:

		myddb_free(v);
		free_rows_and_columns(t);
	}
}

void myddb_delete (void) {

	int error;
	int *v = NULL;
	MYDDBTABLE *t = get_rows_and_columns(2);

	if (t == NULL)
		return;

	/* verify columns */
	if (verify_requested_columns(t, &error))
		warning(W03, omyddb[error].oselect, RVOID);

	myddb_calloc(int, v, (t->h + 1));

	/* search column and row */
	for (int j = 0; j < omyddb->olot; j++) {
		bool match = false;
		for (int x = 0; !match && x < t->w; x++) /* column index */
			if (!(strcmp(t[x].column, omyddb[j].oselect)))
				for (int y = 0; !match && y <= t->h; y++) { /* row index */
					/* 'insert', error */
					if (y == t->h) {
						warning(W05, omyddb[j].oinsert, RBREAK);
					/* del line */
					} else if (!strcmp(t[coor(x,y)].row, omyddb[j].oinsert)) {
						match = true;
						v[y] = true;
					}
				}
	}

	/* crear table */
	FILE *f = NULL;

	if ((f = fopen(myddb_name, "w"))) {
		/* rewrite column */
		for (int x = 0; x < t->w; x++)
			fprintf(myddb_file, "%s%c", t[x].column, gfret(x, (t->w-1)));

		/* rewrite rows */
		for (int y = 0; y < t->h; y++)
			for (int x = 0; x < t->w; x++)
				if (!v[y])
					fprintf(myddb_file, "%s%c", t[coor(x,y)].row, gfret(x, (t->w-1)));

		myddb_free(v);
		myddb_fclose(f);
	} else
		warning(W06, myddb_name, RVOID);

	free_rows_and_columns(t);
}

void myddb_update (void) {

	int error;
	MYDDBTABLE *t = get_rows_and_columns(2);

	if (t == NULL)
		return;

	if (omyddb->row_match == -1)
		warning(E01, "NULL", RVOID);

	/* verify columns */
	if (verify_requested_columns(t, &error))
		warning(W03, omyddb[error].oselect, RVOID);

	/* search row and column */
	for (int i = 0; i < omyddb->olot; i++)
		for (int x = 0; x < t->w; x++)
			if (!strcmp(omyddb[i].oselect, t[x].column)) {
				strcpy(t[coor(x, omyddb->row_match)].row, omyddb[i].oinsert);
				break;
			}

	/* crear table */
	FILE *f = NULL;

	if ((f = fopen(myddb_name, "w"))) {
		/* rewrite columns */
		for (int i = 0; i < t->w; i++)
			fprintf(myddb_file, "%s%c", t[i].column, gfret(i, (t->w-1)));
		/* rewrite row */
		for (int y = 0; y < t->h; y++)
			for (int x = 0; x < t->w; x++)
				fprintf(myddb_file, "%s%c", t[coor(x,y)].row, gfret(x, (t->w-1)));
		fclose(f);
	} else
		warning(W06, myddb_name, RVOID);

	free_rows_and_columns(t);
}

void myddb_cross (void) {

	int error;
	MYDDBTABLE *t = get_rows_and_columns(2);
	omyddb->row_match = -1;

	if (t == NULL)
		return;

	/* verify columns */
	if (verify_requested_columns(t, &error))
		warning(W03, omyddb[error].oselect, RVOID);

	for (int j = 0; j < omyddb->olot; j++) {
		/* search column and row */
		for (int i = 0; i < t->w; i++) {
			bool match = false;
			/* search column  */
			if (!(strcmp(t[i].column, omyddb[j].oselect)))
				for (int y = 0; !match && y < t->h; y++)
					for (int x = 0; !match && x < t->w; x++)
						/* search row */
						if (!strcmp(t[coor(x,y)].row, omyddb[j].oinsert) || *omyddb[j].oinsert == ETX) {
							/* NOTE: cross, it will only malloc omyddb[j].out */
							omyddb[j].out = strdup(t[coor(i,y)].row);
							match = true;
							omyddb->row_match = y;
						}
		}
	}

	free_rows_and_columns(t);
}

void myddb_column (void) {

	int extra = 0;
	MYDDBTABLE *t = get_rows_and_columns(2);

	if (t == NULL)
		return;

	/* verify existent column */
	for (int j = 0; j < omyddb->olot; j++) {
		for (int i = 0; i <= t->w; i++) {
			/* fill columns */
			if (i == t->w) {
				if (*omyddb[j].oinsert == ETX) {
					int e = (t->w + extra);
					/* add more memory to the table */
					if ((t->w * t->h) < omyddb->olot)
						myddb_realloc(MYDDBTABLE, t, (t->w + omyddb->olot));
					t[e].column = strdup(omyddb[j].oselect);
					extra++;
				}
			} else if (!strcmp(omyddb[j].oselect, t[i].column)) {
				/* update column */
				if (*omyddb[j].oinsert != ETX) {
					/* verify existent new column */
					int k;
					for (k = 0; k < t->w; k++)
						if (!strcmp(omyddb[j].oinsert, t[k].column))
							warning(W04, omyddb[j].oinsert, RBREAK)
					/* update */
					if (k == t->w) {
						printf("\n [!] myddb: col update (\"%s\" -> \"%s\")\n", t[i].column, omyddb[j].oinsert);
						myddb_free(t[i].column);
						t[i].column = strdup(omyddb[j].oinsert);
					}
					break;
				} else /* error */
					warning(W04, omyddb[j].oselect, RBREAK)
			}
		}
	}

	if ((myddb_file = freopen(myddb_name, "w", myddb_file))) {
		/* rewrite columns */
		for (int i = 0; i < t->w + extra; i++)
			fprintf(myddb_file, "%s%c", t[i].column, gfret(i, ((t->w + extra) - 1)));
		/* rewrite row */
		for (int y = 0; y < t->h; y++)
			for (int x = 0; x < (t->w + extra); x++)
				fprintf(myddb_file, "%s%c", (x < t->w ? t[coor(x, y)].row : ""), gfret(x, (t->w + extra) - 1));
		myddb_file = freopen(myddb_name, "r+", myddb_file);
	}

	if (extra)
		printf("\n [!] myddb: column(s) created\n");

	/* filled file */
	myddb_fempty = true;
	free_rows_and_columns(t);
}

void myddb_jump (void) {

	int error;
	int target = 0;
	int *v = NULL;
	MYDDBTABLE *t = get_rows_and_columns(2);

	if (t == NULL)
		return;

	/* verify columns */
	if (verify_requested_columns(t, &error))
		warning(W03, t[error].oselect, goto jump_exit);

	myddb_calloc(int, v, (t->w + 1));
	for (int i = 0; i < t->w; i++)
		v[i] = i;

	/* find the order of the columns */
	for (int i = 0, j; i < omyddb->olot; i++) {
		/* find static column index */
		for (j = 0, target = 0; j < t->w; j++)
			if (!strcmp(t[j].column, omyddb[i].oselect))
				break;
		/* column index in vector */
		while (v[target] != j)
			target++;
		/* sort columns */
		int pos = atoi(omyddb[i].oinsert);
		int mov = ((pos - target) < 0 ? -1 : 1);
		for (int k = target; k >= 0 && k < t->w && k != pos; k += mov) {
			int aux = v[k];
			v[k] = v[k + mov];
			v[k + mov] = aux;
		}
	}

	/* rewrite columns */
	for (int i = 0; i < t->w; i++)
		fprintf(myddb_file, "%s%c", t[v[i]].column, gfret(i, (t->w - 1)));

	/* rewrite row */
	for (int y = 0; y < t->h; y++)
		for (int x = 0; x < t->w; x++)
			fprintf(myddb_file, "%s%c", t[coor(v[x], y)].row, gfret(x, (t->w - 1)));

jump_exit:

	free(v);
	free_rows_and_columns(t);
}

void myddb_drop (void) {

	int error;
	int col = 0;
	int *v = NULL;
	MYDDBTABLE *t = get_rows_and_columns(2);

	if (t == NULL)
		return;

	/* erase table */
	FILE *f = NULL;
	if ((f = fopen(myddb_name, "w")))
		fclose(f);
	else
		warning(W12, myddb_name, RVOID);

	/* verify columns */
	if (verify_requested_columns(t, &error))
		warning(W03, omyddb[error].oselect, RVOID);

	myddb_calloc(int, v, (t->w + 1))

	/* drop column */
	for (int i = 0; i < t->w; i++)
		for (int j = 0; j < omyddb->olot; j++)
			if (!strcmp(omyddb[j].oselect, t[i].column)) {
				v[i] = true;
				col++;
				break;
			}

	col = (t->w - col);

	/* rewrite columns */
	for (int i = 0, n = 0; i < t->w; i++)
		if (!v[i])
			fprintf(myddb_file, "%s%c", t[i].column, gfret(++n, col));

	/* rewrite row */
	for (int y = 0; y < t->h; y++)
		for (int x = 0, n = 0; x < t->w; x++)
			if (!v[x])
				fprintf(myddb_file, "%s%c", t[coor(x,y)].row, gfret(++n, col));
			else
				*t[coor(x,y)].row = 0;

	free(v);
	free_rows_and_columns(t);
}

void myddb_create (const char *file) {

	FILE *fcreate = NULL;
	char *buf = NULL;
	myddb_calloc(char, buf, DDBMAX);
	/* verify extent .ddb */
	treat_file_path(buf, file);
	/* verify existence */
	if ((fcreate = fopen(buf, "r"))) {
		fclose(fcreate);
		warning(W13, file, goto create_exit);
	}
	/* create file*/
	if ((fcreate = fopen(buf, "w"))) {
		printf("\n [!] myddb: table \"%s\" created\n", file);
		fclose(fcreate);
	} else
		warning(W13, file, goto create_exit);

create_exit:

	free(buf);
}

void myddb_erase (const char *file) {

	FILE *ferase;
	char *buf = NULL;
	myddb_calloc(char, buf, DDBMAX);
	/* check open file */
	myddb_fclose(myddb_file);
	/* check type file */
	treat_file_path(buf, file);
	/* check file existence */
	if ((ferase = fopen(buf,"r")))
		fclose(ferase);
	else
		warning(W06, buf, goto erase_exit);
	/* erase file */
	if ((ferase = fopen(buf,"w")))
		fclose(ferase);
	else
		warning(W12, buf, goto erase_exit);
	printf("\n [!] myddb: table \"%s\" erased\n", file);

erase_exit:

	free(buf);
}

void myddb_exclude (const char *file) {

	FILE *fexclude = NULL;
	char *buf = NULL;
	myddb_calloc(char, buf, DDBMAX);
	/* check open file */
	myddb_fclose(myddb_file);
	/* check type file */
	treat_file_path(buf, file);
	/* check file existence */
	if (!(fexclude = fopen(buf,"r"))) {
		warning(W06, buf, goto exclude_exit);
	} else
		fclose(fexclude);
	/* exclude */
	if (!remove(buf))
		printf("\n [!] myddb: table \"%s\" excluded\n", file);
	else
		printf("\n [!] myddb: fail to exclude \"%s\"\n", file);

exclude_exit:

	free(buf);
}

void myddb_print (int fmt) {

	MYDDBTABLE *t = NULL;

	/* set limit format */
	fmt = (fmt > 2 ? 2 : (fmt < 0 ? 0 : fmt));

	if ((t = get_rows_and_columns(2)) != NULL) {

		int j = 0;
		(void) j;
		char *bar = NULL;
		char *tmp = NULL;

		myddb_malloc(char, bar, DDBMAX);
		myddb_calloc(char, tmp, DDBMAX);
		myddb_memset(char, bar, '-', DDBMAX);

	#if !NIX_OS
		/* UTF-8 output */
		SetConsoleOutputCP(CP_UTF8);
	#endif

		/* create bar */
		*bar = '+';
		if (fmt == 2)
			bar[j += t->max_line + 1] = '+';
		for (int i = 0; i < (fmt < 2 ? t->w : t->h); i++) {
			if (fmt == 0) bar[j += t[i].col_size + 1] = '+';
			if (fmt == 1) bar[j += t[i].max_size + 1] = '+';
			if (fmt == 2) bar[j += t[i + 1].max_line + 1] = '+';
		}
		bar[j + 1] = '\0';

		/* show columns */
		if (fmt < 2) {
			printf("\n%s\n|", bar);
			for (int i = 0; i < t->w; i++) {
				if (fmt == 0) printf("%-*s|", t[i].col_size + t[i].col_utf8, t[i].column);
				if (fmt == 1) printf("%-*s|", t[i].max_size + t[i].col_utf8, t[i].column);
			}
		}

		printf("\n%s\n", bar);
		for (int k = 0; k < (fmt < 2 ? t->h : t->w); k++) {

			/* column list */
			if (fmt == 2)
				printf("|%-*s", t->max_line + t[k].col_utf8, t[k].column);

			for (int i = 0; i < (fmt < 2 ? t->w : t->h); i++) {
				/* set row */
				if (fmt != 2) j = coor(i, k);
				if (fmt == 2) j = coor(k, i);
				/* short */
				if (fmt == 0) {
					t[j].row[t[i].col_size + t[j].row_utf8] = 0;
					snprintf(tmp, DDBMAX, "%s%*s|", strcat(tmp, t[j].row), (t[i].col_size - (int) strlen(t[j].row)) + t[j].row_utf8, "");
				}
				/* long */
				if (fmt == 1) snprintf(tmp, DDBMAX, "%s%*s|", strcat(tmp, t[j].row), (t[i].max_size - t[j].row_size), "");
				/* list */
				if (fmt == 2) snprintf(tmp, DDBMAX, "%s%*s|", strcat(tmp, t[j].row), (t[i + 1].max_line - t[j].row_size), "");
			}
			printf("|%s\n", tmp); /* single line, fast print */
			*tmp = 0;
		}
		printf("%s\n", bar);

		myddb_free(bar);
		myddb_free(tmp);
		free_rows_and_columns(t);

	/* return console windows state */
	#if !NIX_OS
		SetConsoleOutputCP(output_state_shell);
	#endif
	}
}