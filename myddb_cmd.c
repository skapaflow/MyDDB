#include "src/myddb.h"

int main (int argc, char const *argv[]) {
	/*
		gcc -o myddb_cmd.exe myddb_cmd.c src/myddb.c -s -Wall
	*/
	char *str = malloc(sizeof(char) * DDBMAX);

	myddb_init();

	if (argc > 1) {
		*str = '\0';
		for (int i = 1; i < argc; i++) {
			strcat(str, argv[i]);
			strcat(str, " ");
		}
		myddb(str);
		goto main_exit;
	}

	for (int i = 0; myddb_logo[i]; i++)
		printf(" %s\n", myddb_logo[i]);
	putchar(10);

	while (true) {

		printf("MyDDB: ");

		fgets(str, DDBMAX, stdin);
		str[strlen(str)-1] = '\0';

		if (!strcmp(str, "close") || *str == '.')
			break;

		myddb(str);

		for (int i = 0; omyddb && (i < omyddb->olot); i++)
			printf(" >>> %s\n", omyddb[i].out);
		putchar(10);
	}

main_exit:

	free(str);
	myddb_close();

	return EXIT_SUCCESS;
}
