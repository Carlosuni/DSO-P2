#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/filesystem.h"

#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"

int main() {
	int ret, i, fd;
	char buffer1[BLOCK_SIZE];
	char buffer2[BLOCK_SIZE];
	char buffer3[BLOCK_SIZE];
	//PRUEBAS DE MKFS
	fprintf(stdout, "%s", "TEST mkFS\n");
	//PRUEBA DE MKFS ERRONEA POR TAMAÑO MENOR
	printf("\t\tPRUEBA MKFS 1\n");
	ret = mkFS(5, 327679);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	//PRUEBA DE MKFS ERRONEA POR TAMAÑO MAYOR
	printf("\t\tPRUEBA MKFS 2\n");
	ret = mkFS(5, 512001);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	printf("\t\tPRUEBA MKFS 3\n");
	//PRUEBA DE MKFS ERRONEA POR NUMERO DE ARCHIVOS
	ret = mkFS(-1, 327680);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	printf("\t\tPRUEBA MKFS 4\n");
	//PRUEBA DE MKFS ERRONEA POR NUMERO DE ARCHIVOS
	ret = mkFS(51, 327680);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	printf("\t\tPRUEBA MKFS 5\n");
	//PRUEBA DE MKFS CORRECTA
	ret = mkFS(50, 327680);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	printf("\t\tPRUEBA MKFS 6\n");
	//PRUEBA DE MKFS CORRECTA
	ret = mkFS(1, 512000);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	//PRUEBAS DE MOUNTFS
	printf("\t\tPRUEBA MOUNTFS\n");
	fprintf(stdout, "%s", "TEST mountFS\n");

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	//PRUEBAS DE CREATEFS
	fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST creatFS\n");
	printf("\t\tPRUEBA CREATEFS 1\n");
	ret = creatFS("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	//CREAR UN FICHERO EN UN DISCO DONDE SOLO ENTRA UN FICHERO
	printf("\t\tPRUEBA CREATEFS 1\n");
	ret = creatFS("test1.txt");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	//CREAR 2 FICHEROS CON EL MISMO NOMBRE
	printf("\t\tPRUEBA CREATEFS 2\n");
	ret = mkFS(50, 512000);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	ret = creatFS("test1.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	ret = creatFS("test1.txt");
	if(ret != 1) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	//CREAR DOS FICHERO
	printf("\t\tPRUEBA CREATEFS 3\n");
	ret = creatFS("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	ret = creatFS("test2.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	//ABRIR DOS FICHEROS CON EL MISMO NOMBRE
	fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	printf("\t\tPRUEBA OPENFS 1\n");
	fprintf(stdout, "%s", "TEST openFS + closeFS\n");

	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}
	//EL FICHERO test.txt YA ESTA ABIERTO
	int fd1 = openFS("test.txt");
	if(fd1 != -2) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}
	printf("\t\tPRUEBA OPENFS 2\n");
	//EL FICHERO test2.txt NO EXSISTE
	fd = openFS("test3.txt");
	if(fd != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}
	ret = closeFS(fd);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}
	//PRUEBAS PARA READFS
	fprintf(stdout, "%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	;
	//+ writeFS + lseekFS + closeFS
	fprintf(stdout, "%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS\n");

	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;	
	}
	//leer un archivo inexistente
	printf("\t\tPRUEBA READ 1\n");
	ret = readFS(30, buffer1, BLOCK_SIZE);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;	
	}
	//LEER UN ARCHIVO vacio
	printf("\t\tPRUEBA READ 2\n");
	ret = readFS(fd, buffer1, BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;	
	}
	//LEER mas de un bloque
	printf("\t\tPRUEBA READ 3\n");
	memset(buffer2, 't', BLOCK_SIZE);
	ret = writeFS(fd, buffer2, BLOCK_SIZE+1);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
		return -1;	
	}
	ret = lseekFS(fd, 0, FS_SEEK_BEGIN);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	//LEER UN ARCHIVO vacio
	printf("\t\tPRUEBA READ 4\n");
	ret = readFS(fd, buffer1, -1);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;	
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}
	//PRUEBAS PARA WRITEFS
	//prueba write para un archivo que no existe en el sistema
	printf("\t\tPRUEBA WRITEFS 1\n");
	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;	
	}
	ret = writeFS(30, buffer1, BLOCK_SIZE);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;	
	}
	printf("\t\tPRUEBA WRITEFS 2\n");
	ret = writeFS(fd, buffer1, -1);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;	
	}
	//prueba de writefs el bloque esta lleno
	printf("\t\tPRUEBA WRITEFS 3\n");
	ret = lseekFS(fd, 0, FS_SEEK_END);
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	ret = writeFS(fd, buffer1, BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
		return -1;	
	}
	//prueba de escritura correcta
	printf("\t\tPRUEBA WRITEFS 4\n");
	ret = lseekFS(fd, 0, FS_SEEK_BEGIN);
	if(ret == -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	memset(buffer2, 't', BLOCK_SIZE);
	ret = writeFS(fd, buffer2, BLOCK_SIZE+1);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
		return -1;	
	}
	//prueba lseek con un descriptor de fichero que no existe en el sistema
	printf("\t\tPRUEBA LSEEKFS 1\n");
	ret = lseekFS(30, 0, FS_SEEK_BEGIN);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	//
	printf("\t\tPRUEBA LSEEKFS 2\n");
	ret = lseekFS(fd, 0, FS_SEEK_BEGIN);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	printf("\t\tPRUEBA LSEEKFS 3\n");
	ret = lseekFS(fd, 0, FS_SEEK_END);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	printf("\t\tPRUEBA LSEEKFS 4\n");
	ret = lseekFS(fd, -96, FS_SEEK_SET);
	if(ret != 4000) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	printf("\t\tPRUEBA LSEEKFS 5\n");
	ret = lseekFS(fd, 5, FS_SEEK_SET);
	if(ret != 4005) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	printf("\t\tPRUEBA LSEEKFS 6\n");
	ret = lseekFS(fd, 92, FS_SEEK_SET);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	printf("\t\tPRUEBA LSEEKFS 7\n");
	ret = lseekFS(fd, 4006, FS_SEEK_SET);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	//leer un numero negativo de bytes
	/*memset(buffer2, 't', BLOCK_SIZE);
	ret = writeFS(fd, buffer2, BLOCK_SIZE);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
		return -1;	
	}
	ret = lseekFS(fd, 0, FS_SEEK_SET);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	ret = readFS(fd, buffer3, BLOCK_SIZE);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
		return -1;	
	}
	for(i = 0; i < BLOCK_SIZE; ++i) {
		if(buffer3[i] != 't') {
			fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
			return -1;	
		}
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}*/
	printf("\t\tPRUEBA TAG 1\n");
	fprintf(stdout, "%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST tagFS + listFS + untagFS\n");
	creatFS("test10.txt");
	tagFS("test10.txt", "sample0");
	creatFS("test11.txt");
	tagFS("test11.txt", "sample1");
	creatFS("test12.txt");
	tagFS("test12.txt", "sample2");
	creatFS("test13.txt");
	tagFS("test13.txt", "sample3");
	creatFS("test14.txt");
	tagFS("test14.txt", "sample4");
	creatFS("test15.txt");
	tagFS("test15.txt", "sample5");
	creatFS("test16.txt");
	tagFS("test16.txt", "sample6");
	creatFS("test17.txt");
	tagFS("test17.txt", "sample7");
	creatFS("test18.txt");
	tagFS("test18.txt", "sample8");
	creatFS("test19.txt");
	tagFS("test19.txt", "sample9");
	creatFS("test20.txt");
	tagFS("test20.txt", "sample10");
	creatFS("test21.txt");
	tagFS("test21.txt", "sample11");
	creatFS("test22.txt");
	tagFS("test22.txt", "sample12");
	creatFS("test23.txt");
	tagFS("test23.txt", "sample13");
	creatFS("test24.txt");
	tagFS("test24.txt", "sample14");
	creatFS("test25.txt");
	tagFS("test25.txt", "sample15");
	creatFS("test26.txt");
	tagFS("test26.txt", "sample16");
	creatFS("test27.txt");
	tagFS("test27.txt", "sample17");
	creatFS("test28.txt");
	tagFS("test28.txt", "sample18");
	creatFS("test29.txt");
	tagFS("test29.txt", "sample19");
	creatFS("test30.txt");
	tagFS("test30.txt", "sample20");
	creatFS("test31.txt");
	tagFS("test31.txt", "sample20");
	creatFS("test32.txt");
	tagFS("test32.txt", "sample20");
	creatFS("test33.txt");
	tagFS("test33.txt", "sample20");
	creatFS("test34.txt");
	tagFS("test34.txt", "sample20");
	creatFS("test35.txt");
	tagFS("test35.txt", "sample20");
	creatFS("test36.txt");
	tagFS("test36.txt", "sample20");
	creatFS("test37.txt");
	tagFS("test37.txt", "sample20");
	creatFS("test38.txt");
	tagFS("test38.txt", "sample20");
	creatFS("test39.txt");
	tagFS("test39.txt", "sample20");
	creatFS("test40.txt");
	tagFS("test40.txt", "sample20");
	printf("\t\tPRUEBA TAG 1\n");
	ret = tagFS("test.txt", "sample");
	printf("\t\tPRUEBA TAG 2\n");
	ret = untagFS("test38.txt", "sample28");
	tagFS("test40.txt", "sample30");
	char **files = (char**) malloc(50*sizeof(char*));
	for(i = 0; i < 50; ++i) {
		files[i] = (char*) malloc(64*sizeof(char));
	}
	listFS("sample20", files);
	ret = mkFS(50, 512000);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST mountFS\n");

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST creatFS\n");

	ret = creatFS("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST openFS + closeFS\n");

	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS\n");

	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;	
	}
	ret = readFS(fd, buffer1, BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;	
	}
	memset(buffer2, 't', BLOCK_SIZE);
	ret = writeFS(fd, buffer2, BLOCK_SIZE);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
		return -1;	
	}
	ret = lseekFS(fd, 0, FS_SEEK_BEGIN);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;	
	}
	ret = readFS(fd, buffer3, BLOCK_SIZE);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
		return -1;	
	}
	for(i = 0; i < BLOCK_SIZE; ++i) {
		if(buffer3[i] != 't') {
			fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
			return -1;	
		}
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST tagFS + listFS + untagFS\n");

	ret = tagFS("test.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS\n");
		return -1;
	}
	ret = listFS("sample", files);
	if(ret != 1 || strcmp(files[0], "test.txt") != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS\n");
		return -1;
	}
	ret = untagFS("test.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS\n");
		return -1;
	}
	ret = listFS("sample", files);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS #2\n");
		return -1;
	}
	fprintf(stdout, "%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST umountFS\n");

	
	ret = umountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST umountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST umountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	

	return 0;
}
