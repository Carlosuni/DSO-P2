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

TipoSuperbloque sbloque;
TipoInodoDisco* datos_inodo;
struct SB sb;
struct i_nodo* datos;
Bloque_datos *bloque_datos;

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{
	/* Obtenemos el numero de bloques segun el tamanyo introducido */
	// int aux = deviceSize/BLOCK_SIZE;
	int num_bloques = deviceSize/BLOCK_SIZE;

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
	sbloque.num_magico = 0x000D5500; 
	sbloque.num_bloques_mapa_datos = num_bloques -1; 
	sbloque.num_bloque_datos = num_bloques - 1; 
	sbloque.num_tot_bloques = num_bloques; 						
	sbloque.num_inodos = MAX_FILES; 		
	sbloque.primer_bloque_datos = 1;
	sbloque.tam_dispositivo = deviceSize;
	/* El resto se inicializan a 0 por defecto */	
			
	/* Mapa de bits de inodos inicializado a 0 (libres) */
	for (int i = 0; i < MAX_FILES; i++){
		bitmap_setbit(sbloque.ibitMap, i, 0);
		sbloque.arraynode[i].fd = -1;			/* Iniciamos los descriptores de inodos a -1 */
	} 

	/* Iniciar los bloques de datos con una posicion y como vacios todos */
	bloque_datos = calloc(sbloque.num_tot_bloques, sizeof(datos));
	for (int j = 1; j < sbloque.num_bloque_datos; j++){
	 	bloque_datos[j].pos_bloq_actual = j; 			/* Enumerar los bloque para tenerlos controlados desde 1 a N */
	 	bloque_datos[j].lleno = '0';				/* Ponerlos todos como vacios */
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
	/* Limpiamos inicialmente el superbloque */
	char* buff = malloc(2048);
	printf("%d\n", sbloque.num_tot_bloques);
	memcpy(&sbloque, buff, sizeof(sbloque));
	printf("%s\n", buff);


	/* Leemos el superbloque  */
	bread(DEVICE_IMAGE, 0, buff);
	printf("%li\n", sizeof(sbloque));
	printf("%s\n", buff);
	printf("%d\n", sbloque.num_tot_bloques);
	
	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	char* buff = malloc(2048);
	memcpy(&sbloque, buff, sizeof(sbloque));

	bwrite(DEVICE_IMAGE, 0, buff);
	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *path)
{
	int val = 0;
	/* Buscar que no exista un fichero que tenga el mismo nombre que el que se quiera crear */
	for (int i = 0; i < sbloque.num_inodos; i++){
		if (strcmp(path, sbloque.arraynode[i].nombre) == 0){
			perror("File already exists");
			return -1;
		}
	}

	/* Buscar un espacio vacio */
	for (int i = 0; i < sbloque.num_inodos; i++){
		val = bitmap_getbit(sbloque.ibitMap, i);
		if (val == '0'){ 								/* comprobamos que el inodo esta libre */
			bitmap_setbit(sbloque.ibitMap, i, 1); 			/* Cambiamos el valor del i-nodo de FREE a BUSY  */
			sbloque.arraynode[i].tamanyo = 0; 				/* Asignamos el tamaño inicial de 0B */
			int aux = check_blq_libre();					/* Llamamos a la funcion para comprobar que haya algun ploque libre */
			if (aux > 0){								/* Comprobamos el valor de la funcion */
				sbloque.arraynode[i].bloqueDirecto = aux;	/* Asignamos dicho bloque como el primero en i-node */
			} else {									/* En caso de que no haya bloques libres salta un error */
				perror("File System can't create more files because all blocks of data are fulled");
				return -2;
			}
			strcpy(sbloque.arraynode[i].nombre, path);	/* Copiamos el nombre del fichero */
			bloque_datos[aux].num_bloques = 1;						/* Indicamos de cuantos bloques se compone el fichero */
			bloque_datos[aux].pos_bloq_actual = aux;					/* Posicion actual del fichero */
			bloque_datos[aux].siguiente_bloq = 0;
			sbloque.bloques_en_uso++;								/* Actualizamos la variable con1 noda mas ocupado */
		}		
	}
	return 0;
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

/* Comprueba si hay blosques libres en el bitmap del superbloque
y devuelve la posiciónd el primero que encuentre */
int check_blq_libre()
{
	for (int i = 0; i < sb.numBloquesMapaDatos; i++)
	{
		if (bloque_datos[i].lleno == '0'){
			return i;
		}
	}
	return -1;
}
