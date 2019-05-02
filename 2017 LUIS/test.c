/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	test.c
 * @brief 	Implementation of the client test routines.
 * @date	01/03/2017
 */

#include <stdio.h>
#include <string.h>
#include "include/filesystem.h"
#include "include/auxiliary.h"


// Color definitions for asserts
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE   "\x1b[34m"

#define N_BLOCKS	25						// Number of blocks in the device
#define DEV_SIZE 	N_BLOCKS * BLOCK_SIZE	// Device size, in bytes


int main() {
	int ret;

	//////////////// SUCCESS ////////////////

	///////

	ret = mkFS(DEV_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = createFile("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	///////

	ret = openFile("test.txt");
	if(ret == -2 || ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////
	ret = getPos(1); // es el primer bloque que ocupa text.txt
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST getPos ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST getPos ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	///////

	char charTest[] = "hola mundo.\n";
	ret = writeFile(1, &charTest, sizeof(charTest));
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
 
	///////
	char testRead[12];
	ret = readFile(1, testRead, sizeof(testRead));
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = lseekFile(1, FS_SEEK_BEGIN, 1000000000);
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseekFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = closeFile(1); //recibe un filedescriptor (en este caso el 1 que es donde esta el test.txt)
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	///////

	ret = removeFile("test.txt"); //elimina el archivo
	if(ret == -2 || ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	///////

	ret = unmountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	/////////////////////////// FAILED /////////////////////////////////

	///////

	ret = mkFS(1); // indicamos un tamaño demasiado pequeño
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
  fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	

	///////
	ret = mkFS(1020000); // indicamos un tamaño demasiado pequeño
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
  fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = mkFS(DEV_SIZE);
	ret = mountFS();
	ret = createFile("test.txt");
	ret = createFile("test.txt"); // ya existe archivo
 if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
  fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = unmountFS();
 

	///////

	ret = mkFS(DEV_SIZE);
	ret = mountFS();
	// Crear 26 archivos en un disco de 25
	ret = createFile("test1.txt");
	ret = createFile("test2.txt");
	ret = createFile("test3.txt");
	ret = createFile("test4.txt");
	ret = createFile("test5.txt");
	ret = createFile("test6.txt");
	ret = createFile("test7.txt");
	ret = createFile("test8.txt");
	ret = createFile("test9.txt");
	ret = createFile("test10.txt");
	ret = createFile("test11.txt");
	ret = createFile("test12.txt");
	ret = createFile("test13.txt");
	ret = createFile("test14.txt");
	ret = createFile("test15.txt");
	ret = createFile("test16.txt");
	ret = createFile("test17.txt");
	ret = createFile("test18.txt");
	ret = createFile("test19.txt");
	ret = createFile("test20.txt");
	ret = createFile("test21.txt");
	ret = createFile("test22.txt");
	ret = createFile("test23.txt");
	ret = createFile("test24.txt");
	ret = createFile("test25.txt");
	ret = createFile("test26.txt");
  if(ret != -2) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
  fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST error createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	

	ret = unmountFS();
	return 0;
}
