#include "src/myddb.h"

int main (void) {
	/*
		gcc -o myddb_test.exe myddb_test.c src/myddb.c -s -Wall
	*/
	int c = 0;

	myddb_init();

	const char myddb_test[] =
		"\n  0 - set directory          1 - create and exclude\n"
		"  2 - erase table            3 - clear screen\n"
		"  4 - version                5 - show\n"
		"  6 - help                   7 - print format 0\n"
		"  8 - print format 1         9 - cross\n"
		" 10 - jump columns          11 - drop\n"
		" 12 - 'add' and 'del' row   13 - update\n"
		" 14 - test size             15 - exit\n";

	while (true) {

		printf("\n ----------------------------------------------");
		printf("%s ----------------------------------------------", myddb_test);
		printf("\n test: ");
		scanf("%d", &c);
		fflush(stdin);

		switch (c) {
			case 0:
				/* set directory */
				myddb("PATH .");
				printf("\n [SET DIRECTORY] \"%s\"\n", myddb_path);
				break;
			case 1:
				/* create and exclude */
				printf("\n [CREATE AND EXCLUDE]\n");
				myddb("CREATE test_data SHOW");
				myddb("EXCLUDE test_data SHOW");
				break;
			case 2:
				/* erase table */
				printf("\n [ERASE TABLE]\n");
				myddb("CREATE test_data SHOW");
				myddb("FROM test_data COL (luna sierra) PRINT 1");
				myddb("FROM test_data ADD (luna = sunset sierra = nightmare) PRINT 1");
				myddb("ERASE test_data SHOW");
				myddb("EXCLUDE test_data SHOW");
				break;
			case 3:
				/* clear screen */
				myddb("CLEAR");
				printf("\n [CLEAR SCREEN]\n");
				break;
			case 4:
				/* show version */
				printf("\n [VERSION]\n");
				myddb("VER");
				break;
			case 5:
				/* show files */
				printf("\n [SHOW]\n");
				myddb("SHOW");
				break;
			case 6:
				/* show help */
				printf("\n [HELP]\n");
				myddb("HELP");
				break;
			case 7:
				/* print format 0 */
				printf("\n [PRINT FORMAT 0]\n");
				myddb("FROM wonderland PRINT 0");
				break;
			case 8:
				/* print format 1 */
				printf("\n [PRINT FORMAT 0]\n");
				myddb("FROM wonderland PRINT 1");
				myddb("FROM Porn PRINT 1");
				myddb("FROM test_data PRINT 1");
				break;
			case 9:
				/* test cross */
				printf("\n [CROSS]\n");
				myddb("FROM wonderland PRINT 1 CROSS (name = girl name = rabbit name = man)");
				for (int i = 0; i < omyddb->olot; i++)
					printf("\n out: \"%s\"", omyddb[i].out);
				break;
			case 10:
				/* jump columns */
				printf("\n [JUMP COLUMNS]\n");
				myddb("FROM wonderland JUMP (name = 4 type = 0) PRINT 1");
				myddb("FROM wonderland JUMP (name = 0 type = 4 color = 1 sex = 2 specie = 3) PRINT 1");
				break;
			case 11:
				/* test drop */
				printf("\n [DROP]\n");
				myddb("CREATE test_data SHOW");
				myddb("ERASE test_data");
				myddb("FROM test_data COL (id str data) PRINT 1");
				for (int i = 0; i < 3; i++)
					myddb("FROM test_data ADD (id = %d str = 'string[%c]' data = 25/06/202%d)", i, (i + 'A'), i);
				myddb("FROM test_data PRINT 1");
				myddb("FROM test_data DROP (str)");
				myddb("FROM test_data PRINT 1");
				myddb("FROM test_data COL (query set vet) PRINT 1");
				myddb("EXCLUDE test_data SHOW");
				break;
			case 12:
				/* add and del new row */
				printf("\n ['ADD' AND 'DEL' ROW]\n");
				myddb("FROM wonderland ADD (name = 'Mary Ann' color = 'rose' sex = 'female' specie = 'human' type = 'woman') PRINT 1");
				myddb("FROM wonderland DEL (color = rose) PRINT 1");
				break;
			case 13:
				/* update */
				printf("\n [UPDATE]\n");
				myddb("FROM wonderland WHERE (name = yellow) UPDATE (specie = AI name = Devon color = gray) PRINT 1");
				myddb("FROM wonderland WHERE (name = gray) UPDATE (specie = human name = Alice color = yellow) PRINT 1");
				break;
			case 14:
				/* test size */
				printf("\n [TEST SIZE]\n");
				myddb("CREATE test_data SHOW");
				myddb("FROM test_data COL (TTT0 TTT1 TTT2 TTT3 TTT4 TTT5 TTT6 TTT7 TTT8 TTT9) PRINT 1");
				for (int i = 0, j = 0; i <= 9000; i++) {
					if (j++ > 100) {
						j = 0;
						printf("\r add_iter[%d]", i);
					}
					myddb("FROM test_data ADD (TTT0=%d TTT1=%d TTT2=%d TTT3=%d TTT4=%d TTT5=%d TTT6=%d TTT7=%d TTT8=%d TTT9=%d)" , i, i, i, i, i, i, i, i, i, i);
				}
				myddb("FROM test_data PRINT 1 SHOW EXCLUDE test_data");
				break;
			default: goto test_exit; break;
		}
	}

test_exit:

	printf("\n [END TEST]\n");

	myddb_close();

	return EXIT_SUCCESS;
}
