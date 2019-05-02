/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */

#include "include/filesystem.h" // Headers for the core functionality
#include "include/auxiliary.h"  // Headers for auxiliary functions
#include "include/metadata.h"   // Type and structure declaration of the file system
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SB sb;
struct i_nodo * datos;
struct i_nodo * datos2; //segundo bloque de inodos

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */

int mkFS(long deviceSize)
{
	/* Obtenemos el numero de bloques segun el tamanyo introducido */
	int num_bloques = deviceSize/BLOCK_SIZE;
	//printf("inodo array size: %ld\n", sizeof(struct i_nodo)*40);
	//printf("sb size %ld\n", sizeof(struct SB));

	/* Comprueba que el tamanyo del device llega al minimo */
	if (deviceSize < MIN_HDD){
		perror("FileSystem does not reach the min Size of 50KiB");
		return -1; 
	}

	/* Comprueba que el tamanyo del device no supera el maximo */
	if (deviceSize > MAX_HDD){
		perror("FileSystem exceeding the max Size of 10MiB");
		return -1; 
	}

	/* Guardamos los atributos del struct TipoSuperbloque de metadata.h */
	sb.num_magico = 0x000D5500; 
	sb.tam_dispositivo = deviceSize;
	sb.num_inodos = MAX_FILES;
	sb.numero_bloques_totales = num_bloques;
	sb.num_bloques_Datos = num_bloques - 3;

	/* Generamos la estructura de inodos de forma dinamica, por defecto 40 inodos */
	datos = malloc(sizeof(struct i_nodo)*35);
	for(int i=0;i<=35-1;i++){
		datos[i].usado=0;
		datos[i].id=i;
	}
	datos[0].isDir=1;
	datos[0].usado=1;
	strcpy(datos[0].nombre, "/");
	
	datos2 = malloc(sizeof(struct i_nodo)*5);
	for(int i=0;i<=5-1;i++){
		datos2[i].usado=0;
		datos[i].id=35+i;
	}

	bmap = malloc(sizeof(char)*sb.numero_bloques_totales);
	inodos = malloc(sizeof(char)*MAX_FILES);
	bmap[0]='1';
	bmap[1]='1';
	bmap[2]='1';
	printf("%s\n", bmap);
	inodos[0]='1';

	/* Invocamos umount */
	unmountFS();

	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{	
	/* Limpiamos inicialmente el superbloque */
	char* buff = malloc(2048);
	
	/* Leemos el superbloque  */
	bread(DEVICE_IMAGE, 0, buff);
	memcpy(&sb, buff, sizeof(sb));

	bread(DEVICE_IMAGE, 1, buff);
	memcpy(buff, &datos, sizeof(struct i_nodo));	

	bread(DEVICE_IMAGE, 2, buff);
	memcpy(buff, &datos2, sizeof(struct i_nodo));

	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	char* buff = malloc(2048);
	memcpy(buff, &sb, sizeof(struct SB));

	bwrite(DEVICE_IMAGE, 0, buff);

	memcpy(buff ,datos ,sizeof(struct i_nodo));	
	bwrite(DEVICE_IMAGE, 1, buff);

	memcpy(buff ,datos2 , sizeof(struct i_nodo));	
	bwrite(DEVICE_IMAGE, 2, buff);
	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *path)
{
	int libre = contar_ficheros();
	int blib = obtener_blq_libre();
	printf("bloque l: %d\n", blib);
	printf("%d\n", libre);
	if(libre==0){
		int ele = separar_path(path);
		printf("%d\n", ele);
		return 0;
	}else{
		return -1;
	}
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *path)
{
	return -2;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *path)
{
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	return -1;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	return -1;
}

/*
 * @brief	Creates a new directory provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the directory already exists, -2 in case of error.
 */
int mkDir(char *path)
{
	return -2;
}

/*
 * @brief	Deletes a directory, provided it exists in the file system.
 * @return	0 if success, -1 if the directory does not exist, -2 in case of error..
 */
int rmDir(char *path)
{
	return -2;
}

/*
 * @brief	Lists the content of a directory and stores the inodes and names in arrays.
 * @return	The number of items in the directory, -1 if the directory does not exist, -2 in case of error..
 */
int lsDir(char *path, int inodesDir[10], char namesDir[10][33])
{
	char * tok = nombre_directorio(path);
	printf("%s\n", tok);
	return -2;
}

/* Comprueba si hay blosques libres en el bitmap del superbloque
y devuelve la posiciónd el primero que encuentre */
int obtener_blq_libre()
{	
	int i=0;
	for(i=0;i<=sb.numero_bloques_totales-1;i++){
		/*if(strcmp(bmap[i], "0")==0){
			return i;
		}*/
		return -1;
	}
	return -1;
}

// devuelve 0 si se puede añadir un fichero, sino devuelve -1;
int contar_ficheros(){
	int ficheros=0;
	int i=0;
	for(i=0;i<=35-1;i++){
		if(datos[i].isDir==0 && datos[i].usado==1){
			if(ficheros<MAX_FILES){
				ficheros++;
			}else{
				return -1;
			}
			
		}
	}
	for(i=0;i<=35-1;i++){
		if(datos2[i].isDir==0 && datos[i].usado==1){
			if(ficheros<MAX_FILES){
				ficheros++;
			}else{
				return -1;
			}
		}
	}
	return 0;
}

//devuelve la longitud a la que se encuentra el archivo
int separar_path(char * path){
	int items=1;
	char * tokens = strtok(path,"/");
	while( tokens != NULL ) {
    	printf( " %s\n", tokens );
		items++;
    	tokens = strtok(NULL,"/");
   	}
	return items;
}

char * nombre_directorio(char * path){
	char * token = strtok(path,"/");
	while( token != NULL ) {
    	printf( " %s\n", token );
    	token = strtok(NULL,"/");
   	}
	return token;
}


