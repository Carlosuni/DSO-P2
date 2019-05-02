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
TipoInodoDisco inodos[MAX_FILES];
Bloque_datos bloques_datos_aux;
/* Preparado en caso de que hubiera mas 40 inodos y de 1 bloque de mapa de inodos*/
Bloque_datos bloques_inode_map[BLOCK_SIZE/40+1];
/* Preparado en caso de que hubiera mas 40 inodos y de 1 bloque de mapa de inodos*/
Bloque_datos bloques_datos_map[25 - 4];

char ibit_map[2048];			/* Mapa de bits inodos 5B */
char bbit_map[2048];			/* Mapa de bits bloques 5B */


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
	sbloque.num_bloques_mapa_inodos = 1; 
	sbloque.num_bloques_mapa_datos = 1; 
	/* Iniciamos el numero d einodos a 1, ya que será variable
	para optimizar el espacio */
	sbloque.num_bloque_datos = 0;
	sbloque.num_tot_bloques = num_bloques; 
	sbloque.primer_bloque_datos = 0;       /* Num de bloque del primer bloque de datos */
	sbloque.tam_dispositivo = deviceSize;
	sbloque.max_inodos = MAX_FILES;

	/* Asignando el máximo de espacio restante a bloques de datos y su mapa */
	// int bloques_restantes = sbloque.num_tot_bloques - 1 - sbloque.num_bloques_mapa_inodos - sbloque.num_inodos;
	// printf("%u\n", sbloque.num_tot_bloques);
	// printf("%d\n", bloques_restantes);
	//sbloque.num_bloque_datos = (sbloque.num_tot_bloques*2048 - 2048 - sbloque.num_bloques_mapa_inodos*2048) / 2049; 

	/* Preparamos a 0 (libres) los dos bitmaps,
	aunque realmente la lista de nodos estará a 0 hasta meter el raíz */
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
	  bitmap_setbit(ibit_map, i, 0);	// libre
	  //if (i < 300)
	  //printf("bit: pos = %d, val = %x\n", i, bitmap_getbit(ibit_map, i));

	}
	for (int i = 0; i < BLOCK_SIZE; i++)
	  bitmap_setbit(bbit_map, i, 0);	// libre
	for (int i = 0; i < sbloque.num_inodos; i++)
	  memset(&(inodos[i]), 0, sizeof(TipoInodoDisco));
	
	char diskbloq_bit_map[BLOCK_SIZE];			/* Mapa de bits inodos 5B */
	for (int i = 0; i < BLOCK_SIZE * 8; i++)
	{
	  bitmap_setbit(diskbloq_bit_map, i, 0);	// libre
	  //printf("bit: pos = %d, val = %x\n", i, bitmap_getbit(diskbloq_bit_map, i));
	}

	/* Reseteamos el disco */
	for (int i = 0; i < 200; i++)
	  bwrite(DEVICE_IMAGE, i, diskbloq_bit_map);

	
	/* Creamos inodo del directorio raiz */
	strcpy(inodos[0].nombre, "/");
	inodos[0].tipo = 1;
	inodos[0].num_bloque_inodo = 3;
	sbloque.num_inodos = 1;
	sbloque.primer_inodo = 3;              /* Num de bloque del primero inodo del disp (raiz) */	
	sbloque.bloques_en_uso = 4;
	//printf("Bitmap 1 = %d\n", sbloque.ibit_map[0]);

	bitmap_setbit(ibit_map, 0, 1);	// usado
	//printf("Bitmap 1 = %d\n", sbloque.ibit_map[0]);

	/* Guardamos las posiciones de los primeros bloques de los mapas */
	bloques_inode_map[0].pos_actual_bloq = 1;
	bloques_datos_map[0].pos_actual_bloq = 2;
	//bloques_inode_map[0].num_bloques = 1;
	
	/* Preparamos bloque auxiliar con mapa de datos */
	//size_t tam_bloq_inodemap = sizeof(bloques_inode_map)/sizeof(bloques_inode_map[0]);
	//bloques_datos_map[(sbloque.num_tot_bloques - 1 - tam_bloq_inodomap) / 2];

			
	// /* Mapa de bits de inodos inicializado a 0 (libres) */
	// for (int i = 0; i < MAX_FILES; i++){
	// 	bitmap_setbit(sbloque.ibitMap, i, 0);
	// 	sbloque.arraynode[i].fd = -1;			/* Iniciamos los descriptores de inodos a -1 */
	// } 

	/* Iniciar los bloques de datos con una posicion y como vacios todos */
	// bloque_datos = calloc(sbloque.num_tot_bloques, sizeof(datos));
	// for (int i = 1; i < sbloque.num_bloque_datos; i++){
	//  	bloque_datos[i].pos_bloq_actual = i; 			/* Enumerar los bloque para tenerlos controlados desde 1 a N */
	//  	bloque_datos[i].lleno = '0';				/* Ponerlos todos como vacios */
	//  	/* No hace falta poner valores a los demas atributos de la estructura porque 
	//  		ya se han inicializado a 0 con la funcion calloc*/
	// }

	disk_sync();

	// /* Invocamos umount */
	// unmountFS();

	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{	
	char buff[2048];
	// leer bloque 0 con superbloque
	bread(DEVICE_IMAGE, 0, buff);
	memcpy(&sbloque, buff, sizeof(buff));

	int bloque_actual = 1;
	for (int i = 0; i < sbloque.num_bloques_mapa_inodos; i++)
	{
	  bread(DEVICE_IMAGE, bloque_actual, buff); 
	  memcpy(ibit_map, buff, sizeof(buff));
	  bloque_actual = bloques_inode_map[0].siguiente_bloq;
	  //printf("%x\n", bitmap_getbit(ibit_map, 0));
	}

	bloque_actual = 2;
	for (int i = 0; i < sbloque.num_bloques_mapa_datos; i++)
	{
	  bread(DEVICE_IMAGE, bloque_actual, buff); 
	  memcpy(bbit_map, buff, sizeof(buff));
	  bloque_actual = bloques_datos_map[0].siguiente_bloq;
	  //printf("%x\n", bitmap_getbit(bbit_map, 0));
	}

	bloque_actual = 3;
	for (int i = 0; i < sbloque.num_inodos; i++)
	{
	  bread(DEVICE_IMAGE, bloque_actual, buff); 
	  memcpy(&inodos[0], buff, sizeof(buff));
	  bloque_actual = inodos[0].bloque_next_inodo;
	  //printf("%s\n", inodos[0].nombre);
	}
		
	// leer los i-nodos a memoria
	for (int i=0; i<(sbloque.num_inodos*sizeof(TipoInodoDisco)/BLOCK_SIZE); i++)
		bread(DEVICE_IMAGE, i+sbloque.primer_inodo, ((char *)inodos + i*BLOCK_SIZE));

	// /* Limpiamos inicialmente el superbloque */
	// char* buff = malloc(2048);
	// //printf("%d\n", sbloque.num_tot_bloques);
	// memcpy(&sbloque, buff, sizeof(sbloque));
	// //printf("%s\n", buff);


	// /* Leemos el superbloque  */
	// bread(DEVICE_IMAGE, 0, buff);
	// //printf("%li\n", sizeof(sbloque));
	// //printf("%s\n", buff);
	// //printf("%d\n", sbloque.num_tot_bloques);

	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	// asegurarse de que todos los ficheros están cerrados
	for (int i = 0; i < sbloque.num_inodos; i++) {
	  if (inodos[i].en_uso == 1) {
		  perror ("No es posible desmontar el sistema de ficheros. Hay nos en uso");
	    return -1;
	  }
	}
	
	// escribir a disco los metadatos
	sync();

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *path)
{
	int new_b_id, new_bloqinode_id, new_inodo_id ;

	new_inodo_id = ialloc_custom();
	if (new_inodo_id < 0)
	{
		return new_inodo_id;
	}
	
	new_bloqinode_id = alloc_custom();
	if (new_bloqinode_id < 0) { ifree_custom(new_inodo_id); return new_b_id ; }
	new_b_id = alloc_custom();
	if (new_b_id < 0) { ifree_custom(new_inodo_id); return new_b_id ; }
	strcpy(inodos[new_inodo_id].nombre, path);
	bloques_datos_map[new_bloqinode_id].pos_actual_bloq = 2;

	inodos[new_inodo_id].num_bloque_inodo = new_bloqinode_id;
	inodos[new_inodo_id - 1].bloque_next_inodo = new_bloqinode_id;
	inodos[new_inodo_id - 1].num_contenidos = 1;
	inodos[new_inodo_id - 1].inodosContenidos[inodos[new_inodo_id - 1].num_contenidos - 1] = new_bloqinode_id;
	inodos[new_inodo_id].bloqueDirecto = new_b_id;


	// inodos[inodo_id].bloqueDirecto = new_b_id ;
	// inodos_x[inodo_id].posicion = 0;
	// inodos_x[inodo_id].abierto = 1;

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
// int check_blq_libre()
// {
// 	for (int i = 0; i < sb.numBloquesMapaDatos; i++)
// 	{
// 		if (bloque_datos[i].lleno == '0'){
// 			return i;
// 		}
// 	}
// 	return -1;
// }

int disk_sync()
{
	// // escribir superbloque a disco
	char buff[2048];
	// //printf("tamanyo buff = %lu\n", sizeof(buff));
	//printf("tamanyo sbloque = %lu\n", sizeof(sbloque));
	memcpy(buff, &sbloque, sizeof(sbloque));
	bwrite(DEVICE_IMAGE, 0, buff);
	//printf("tamanyo buff = %lu\n", sizeof(buff));

	// escribir los bloques para el mapa de i-nodos
	int bloque_actual = bloques_inode_map[0].pos_actual_bloq;
	//printf("Bitmap 1 = %d\n", sbloque.ibit_map[0]);
	for (int i = 0; i < sbloque.num_bloques_mapa_inodos; i++)
	{
	  //printf("escribiendo mapa inodo\n");
	  //printf("ibit_map escrito\n%s", (char *)ibit_map);
	  bwrite(DEVICE_IMAGE, bloque_actual, ((char *)ibit_map));
	  bloque_actual = bloques_inode_map[0].siguiente_bloq;
	}

	// escribir los bloques para el mapa de bloques de datos
	bloque_actual = bloques_datos_map[0].pos_actual_bloq;
	for (int i = 0; i < sbloque.num_bloques_mapa_datos; i++)
	{
	  bwrite(DEVICE_IMAGE, bloque_actual, ((char *)bbit_map));
	  bloque_actual = bloques_datos_map[0].siguiente_bloq;
	}
	
	// escribir los i-nodos a disco
	bloque_actual = inodos[0].num_bloque_inodo;
	for (int i = 0; i < sbloque.num_inodos; i++)
	{
	  bwrite(DEVICE_IMAGE, bloque_actual, ((char *) inodos + i * BLOCK_SIZE));
	  bloque_actual = inodos[i].bloque_next_inodo;
	}

	return 0;
}

/* Devuelve el primer inodo libre que encuentra*/
int ialloc_custom(void)
{
	// buscar un i-nodo libre
	for (int i = 0; i < sbloque.num_inodos; i++)
	{
		if (ibit_map[i] == 0)
		{
			// inodo ocupado ahora
			ibit_map[i] = 1;
			// valores por defecto en el i-nodo
			//memset(&(inodos[i]), 0, sizeof(TipoInodoDisco));
			// devolver identificador de i-nodo
			return i;
		}
	}
	return -1;
}

/* Devuelve el primer bloque libre que encuentra*/
int alloc_custom(void)
{
	char b[BLOCK_SIZE];	
	
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		if (bbit_map[i] == 0) {
			// bloque ocupado ahora
			bbit_map[i] = 1;
			// valores por defecto en el bloque
			//memset(b, 0, BLOCK_SIZE);
			//bwrite(DISK, sbloque.primerBloqueDatos + i, b);
			// devolver identificador del bloque
			if (i >= sizeof(bloques_datos_map)/sizeof(bloques_datos_map[0]))
			{
				printf("No caben mas bloques");
				return -1;
			}
			return i;
		}
	}
	return -1;
}

/* Devuelve el primer bloque libre que encuentra*/
int alloc_databloq_custom(int id_bloque_Datos)
{
	char b[BLOCK_SIZE];	
	
	for (int i = 0; i < sizeof(bloques_datos_map)/sizeof(bloques_datos_map[0]); i++)
	{
		int no_inodo = check_free_inode(id_bloque_Datos);
		check_free_inode
					if (i > 3 && inodos[j].num_bloque_inodo)
			return i;
	}
	return -1;
}

int check_free_inode(int num_bloque)
{
	for (int i = 0; i < sbloque.num_inodos; i++)
	{
		if (inodos[i].num_bloque_inodo)
		{
			return 0;
		}
	}
	return 1;
}