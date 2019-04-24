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
/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{
	int i;

	/*Saber el numero de bloques introducidos*/
	int aux = deviceSize/BLOCK_SIZE;

	/*Comprobar el tamaño minimo y maximo del sistema*/
	if (deviceSize < MIN_HDD){
		perror("FileSystem does not reach the min Size of 50KiB");
		return -1; 
	}
	 if (deviceSize > MAX_HDD){
		perror("FileSystem exceeding the max Size of 10MiB");
		return -1; 
	}

	sb.tamDispositivo = deviceSize;	/* Tamanyo maximo del dispositivo */
	sb.numTotalBloques = aux; 			/* Los bloques introducidos en el proceso test */
	sb.numInodos = MAX_FILES; 		/* Vamos a tener como maximo 40 nodos 1 por cada fichero que el maximo que son es 40*/
	sb.primerBloqueDatos = 1;			/* Tenemos reservado el bloque 0 para el superbloque y los inodos */
	sb.numBloquesMapaDatos = aux -1; 	/* El numero maximo de bloques que podemos usar para guardar datos */

	 /*mapa de bits inicializado a 0 para saber que todos los inodos estan libres*/
	for (i = 0; i < MAX_FILES; i++){
		bitmap_setbit(sb.bitMap, i, 0);
		sb.arraynode[i].usado = -1;			/* Iniciamos los descriptores de fichero asiciados a todos los nodos a -1 */
	} 

	 /* Iniciar los bloques de datos con una posicion y como vacios todos */
	datos = calloc(sb.numBloquesMapaDatos, sizeof(datos));
	for (int j = 1; j < sb.numTotalBloques; j++){
	 	datos[j].position = j; 			/* Enumerar los bloque para tenerlos controlados desde 1 a N */
	 	datos[j].lleno = '0';				/* Ponerlos todos como vacios */
	 	/* No hace falta poner valores a los demas atributos de la estructura porque 
	 		ya se han inicializado a 0 con la funcion calloc*/
	}

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
	char* buff = malloc(2048);
	memcpy(&sb, buff, sizeof(sb));

	/* Leer el SuperBloque  */
	bread(DEVICE_IMAGE, 0, buff);
	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	char* buff = malloc(2048);
	memcpy(&sb, buff, sizeof(sb));

	bwrite(DEVICE_IMAGE, 0, buff);
	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *path)
{
	return -2;
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
	return -2;
}
