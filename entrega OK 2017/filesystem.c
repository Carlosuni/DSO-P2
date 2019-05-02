/* This file contains the definition of the functions that must be implemented
 * to allow the user access to the file system and the files.
 */

#include "include/filesystem.h"
#include <string.h>
#include <stdlib.h>

// Constante de número de ficheros
#define MAX_SIZE 512000

 // Constante de número de ficheros
#define MIN_SIZE 327680

// Constante de número de ficheros
#define MAX_FILES 50

// Constante de caracteres de etiqueta
#define NAME_TAG 32

 // Constante de numero maximo de etiquetas
#define MAX_TAG 30

// Constantes de apertura/cerrado
#define CLOSED '0'
#define OPENED '1'

// Constantes de apertura/cerrado
#define UNTAG '0'
#define TAG '1'

#define META_BLOCKS 2
/***************************/
/* Estructuras de datos.   */
/***************************/

//Definicion de la estructura TAG
 //86B
typedef struct {
	char name[NAME_TAG];		//32B nombre de la etiqueta
	char indexImap[MAX_FILES];	//50B
	int i;						//4B
} tag;


// Definición de la estructura superbloque
// Bloque 0 108
typedef struct { 
	unsigned int inodeNumber; 	//4B Número inodos
	unsigned int deviceSize;  	//4B tam disp
	short iMap[MAX_FILES]; 		//100B Mapa de inodos (0 = no creado, 1 = creado)
} SuperBlock;

// Definición de la estructura del inodo
// 80B
typedef struct {
	unsigned int fileSize; 		//4B
	unsigned int fd; 			//4B
	unsigned int position; 		//4B
	short indexTag;				//2B max 3
	char name[64]; 				//64B	
	char open; 					//1B
	char block; 				//1B
} inode;

// Definiciaón de la lista de i-nodos
// Bloque 1
typedef struct {
	inode inodeArray[MAX_FILES];//4000B
} iList;

//Definicion de la lista de tags
//Bloque 0 2584
typedef struct{
	tag tags[MAX_TAG]; 			//2580B
	int index;					//4B
}tList;

typedef struct{
	SuperBlock sb;				//108B
	tList tl;					//2584B
}Blk0;							//2692B

SuperBlock sB;
iList iL;
tList tL;
Blk0  bZ;
char emptyBlock[BLOCK_SIZE];

int positionFD(int fd){
	int i=0, found=0;
	//Buscar el fichero por su descriptor
	if(fd<1){
		printf("\tFileSystem: positionFD, descriptor de fichero invalido");
		return -1;
	}
	while(i<sB.inodeNumber && found == 0){
		if(iL.inodeArray[i].fd == fd){
			found = 1;
		}else{
			i++;
		}
	}
	//error si no hemos encontrado el fichero
	if (found == 0){
		return -1;
	}
	return i;
}


/***************************/
/* File system management. */
/***************************/

/*
 * Formats a device.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mkFS(int maxNumFiles, long deviceSize) {
	//error si el tamaño del dispositivo es menor de 320kB o mayor de 500kB
	if(deviceSize<MIN_SIZE || deviceSize>MAX_SIZE){
		printf("\tFileSystem: error en mkFS, este sistema de archivos esta preparado para dispositivos entre 320kB y 500kB\n");
		return -1;
	}
	//error si no hay como minimo 1 archivo
	if(maxNumFiles<1){
		printf("\tFileSystem: error en mkFS, numero maximo de archivos invalido\n");
		return -1;
	}
	//error si hay mas de 50 archivos
	if(maxNumFiles>MAX_FILES){
		printf("\tFileSystem: error en mkFS, el numero maximo de archivos no puede ser mayor a 50\n");
		return -1;
	}
	//Crear SuperBloque
	sB.inodeNumber = maxNumFiles;
	sB.deviceSize = deviceSize;
	//inicializar el mapa de inodos y el mapa de tags libres a 0;
	int i;
	for (i=0;i<MAX_FILES;i++){
		sB.iMap[i]=0;
		//sB.tMap[i]=0;
	}
	//iniciar los inodos
	for(i=0;i<maxNumFiles;i++){
		memset(&(iL.inodeArray[i]),0,sizeof(inode));
	}
	//inicializar la taglist
	for(i=0;i<MAX_TAG;i++){
		memset(&(tL.tags[i]),0,sizeof(tag));
	}

	//umount para escribir los bloques de metadatos
	umountFS();

	printf("FileSystem: mkFS, se ha formateado el disco correctamente\n");
	return 0;
}	

/*
 * Mounts a file system from the device deviceName.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mountFS() {
	int ret;

	//leer el superbloque
	ret=bread(DEVICE_IMAGE,0,(char *) &bZ);
	if (ret<0)
		return -1;

	//leer los inodos
	ret=bread(DEVICE_IMAGE,1,(char *) &iL);
	if(ret<0)
		return -1;
	sB=bZ.sb;
	tL=bZ.tl;
	printf("FileSystem: mkFS, se ha montado el disco correctamente\n");
	return 0;
}

/*
 * Unmount file system.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int umountFS() {
	int ret;
	//escribir el superbloque
	bZ.sb = sB;
	bZ.tl = tL;
	ret=bwrite(DEVICE_IMAGE,0,(char*)&bZ);
	if(ret<0)
		return -1;
	//escribir los inodos
	ret=bwrite(DEVICE_IMAGE,1,(char*)&iL);
	if(ret<0)
		return -1;
	printf("FileSystem: mkFS, se ha desmontado el disco correctamente\n");
	return 0;
}

/*******************/
/* File read/write */
/*******************/

/*
 * Creates a new file, if it doesn't exist.
 * Returns 0 if a new file is created, 1 if the file already exists or -1 in
 * case of error.
 */
int creatFS(char *fileName) {
	int newPos,found=0,i;
	//Buscar en el mapa de inodos si hay alguna posicion libre
	newPos=0;
	while(newPos<sB.inodeNumber && found==0){
		if (sB.iMap[newPos]==0)
			found=1;
		else
			newPos++;
	}
	if (found ==0){
		printf("\tFileSystem: error en createFS, no hay espacio para cear un nuevo archivo\n");
		return -1;
	}
	//Buscar en la lista de inodos el nombre por si ya 
	//existiese en el sistema de archivos
	for (i=0;i<sB.inodeNumber;i++){
		if(strcmp(iL.inodeArray[i].name,fileName) == 0){
			printf("\tFileSystem: error en createFS, el archovo %s ya esta creado en el sistema\n", fileName);
			return 1;
		}
	}
	//si hay hueco en el sistema de archivos y no hay ningun otro
	// archivo con el mismo nombre procedemos a crear el nuevo archivo
	sB.iMap[newPos] = 1;
	iL.inodeArray[newPos].block = META_BLOCKS+newPos;
	iL.inodeArray[newPos].open = CLOSED;
	bwrite(DEVICE_IMAGE,iL.inodeArray[newPos].block, (char*) &emptyBlock);//set
	iL.inodeArray[newPos].fd = 0;
	iL.inodeArray[newPos].position = 0;
	iL.inodeArray[newPos].fileSize = 0;
	strcpy(iL.inodeArray[newPos].name,fileName);
	printf("FileSystem: createFS, se ha creado el archivo correctameunte\n");
	return 0;
}

/*
 * Opens an existing file.
 * Returns file descriptor if possible, -1 if file does not exist or -1 in case
 * of error.
 */
int openFS(char *fileName) {
	int i=0, found=0;
	// Recorremos losinodos buscando el archivo pedido
	while(i<sB.inodeNumber && found==0){
		if(strcmp(iL.inodeArray[i].name,fileName) == 0 && sB.iMap[i] == 1){
			found = 1;
		}else{
			i++;
		}
	}
	//si el archivo no ha sido encontrado mandamos un mensaje de error
	if(found == 0){
		printf("\tFileSystem: openFS, el archivo %s no esta en el sistema\n", fileName);
		return -1;
	}
	//si el archivo ya esta abierto mandamos un mensaje de error
	if(iL.inodeArray[i].open == OPENED){
		printf("\tFileSystem: openFS, el archivo %s ya se encuentra abierto\n",fileName);
		return -2;
	}
	//el fichero existe en el sistema y esta cerrado, procedemos 
	//a abrirlo y retornar el descriptor de fichero
	iL.inodeArray[i].position = 0;
	iL.inodeArray[i].open = OPENED;
	//no pueden empezar en 0 por que si no lo confundiriamos con el error de fichero abierto
	iL.inodeArray[i].fd = i + 1; 
	printf("FileSystem: openFS, el archivo ha sido abierto correctamente, descriptor de fichero = %d\n",iL.inodeArray[i].fd );
	return iL.inodeArray[i].fd ;
}

/*
 * Closes a file.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int closeFS(int fileDescriptor) {
	int i;
	i = positionFD(fileDescriptor);
	if(i<0){
		printf("\tFileSystem: closeFS, no se ha encontrado el descriptor de fichero %d\n", fileDescriptor);
		return -1;
	}
	//error si el archivo ya estaba cerrado
	if (iL.inodeArray[i].open == CLOSED){
		printf("\tFileSystem: closeFS, el fichero %d ya estaba cerrado\n", fileDescriptor);
		return -1;
	}
	iL.inodeArray[i].open = CLOSED;
	iL.inodeArray[i].fd =0;
	printf("FileSystem: closeFS, el fichero %s con descriptor de fichero %d se ha cerrado correctamente\n",iL.inodeArray[i].name, fileDescriptor);
	return 0;
}

/*
 * Reads a number of bytes from a file and stores them in a buffer.
 * Returns the number of bytes read or -1 in case of error.
 */
int readFS(int fileDescriptor, void *buffer, int numBytes) {
	int i ;
	i=positionFD(fileDescriptor);
	//error si no hemos encontrado el fichero
	if (i<0){
		printf("\tFileSystem: readFS, no se ha encontrado el descriptor de fichero %d\n", fileDescriptor);
		return -1;
	}
	//error si el numero de bytes a leer es 0 o menor
	if(numBytes<1){
		printf("\tFileSystem: readFS, el numero de Bytes a leer debe ser al menos uno\n");
		return -1;
	}
	//Si la posicion de lectura/escritura del fichero esta al final retornamos 0
	if(iL.inodeArray[i].position == iL.inodeArray[i].fileSize){
		printf("\tFileSystem: readFS, el puntero de lectura/escritura esta al final del fichero, no se puede leer nada\n");
		return 0;
	}
	//Si se piden mas Bytes de lectura que 
	if(iL.inodeArray[i].position+numBytes > iL.inodeArray[i].fileSize){
		numBytes = iL.inodeArray[i].fileSize - iL.inodeArray[i].position;
	}
	//realizamos la lectura
	char aux[BLOCK_SIZE];
	bread(DEVICE_IMAGE, iL.inodeArray[i].block, aux);
	//movemos la posiocion de lectura/escritura
	memmove(buffer, aux+iL.inodeArray[i].position, numBytes);
	iL.inodeArray[i].position = iL.inodeArray[i].position + numBytes;
	printf("FileSystem: readFS, lectura realizada con exito\n");
	return numBytes;
}

/*
 * Reads number of bytes from a buffer and writes them in a file.
 * Returns the number of bytes written, 0 in case of end of file or -1 in case
 * of error.
 */
int writeFS(int fileDescriptor, void *buffer, int numBytes) {
	int i ;
	i=positionFD(fileDescriptor);
	//error si no hemos encontrado el fichero
	if (i<0){
		printf("\tFileSystem: writeFS, no se ha encontrado el descriptor de fichero %d\n", fileDescriptor);
		return -1;
	}
	//Comprobar el numero de Bytes a escribir es mayor que 1
	if (numBytes < 1){
		printf("\tFileSystem: writeFS, el numero de Bytes a escribir debe ser al menos uno\n");
		return -1;
	}
	//Comprobar si la posicion de lectura/escritura esta al finaldel fichero
	if(iL.inodeArray[i].position == BLOCK_SIZE){
		printf("\tFileSystem: writeFS, el puntero de lectura/escritura esta al final del fichero, no se puede escribir nada\n");
		return 0;
	}

	if(iL.inodeArray[i].position + numBytes > BLOCK_SIZE){
		numBytes=BLOCK_SIZE - iL.inodeArray[i].position;
	}
	//realizamos la operacion de escritura
	char aux[BLOCK_SIZE];
	bread(DEVICE_IMAGE, iL.inodeArray[i].block, aux);
	memmove(aux+iL.inodeArray[i].position, buffer, numBytes);
	bwrite(DEVICE_IMAGE, iL.inodeArray[i].block, aux);
	iL.inodeArray[i].position = iL.inodeArray[i].position + numBytes;
	//calcular los nuevos bytes usados en el fichero
	int x;
	for (x=0;x<BLOCK_SIZE;x++){
		if (aux[x]==0)
			break;
	}
	iL.inodeArray[i].fileSize = x;
	printf("FileSystem: writeFS, escritura realizada con exito\n");
	return numBytes;
}


/*
 * Repositions the pointer of a file. A greater offset than the current size, or
 * an offset lower than 0 are considered errors.
 * Returns new position or -1 in case of error.
 */
int lseekFS(int fileDescriptor, long offset, int whence) {
	int i ;
	i=positionFD(fileDescriptor);
	//error si no hemos encontrado el fichero
	if (i<0){
		printf("\tFileSystem: lseekFS, no se ha encontrado el descriptor de fichero %d\n", fileDescriptor);
		return -1;
	}
	if (whence == FS_SEEK_END){
		iL.inodeArray[i].position = iL.inodeArray[i].fileSize;
		printf("FileSystem: lseekFS, se ha movido el puntero de lectura/escritura movido al final del fichero correctamente\n");
		return iL.inodeArray[i].position;
	}
	if (whence == FS_SEEK_BEGIN){
		iL.inodeArray[i].position = 0;
		printf("FileSystem: lseekFS, se ha movido el puntero de lectura/escritura movido al inicio del fichero correctamente\n");
		return iL.inodeArray[i].position;
	}
	if (whence == FS_SEEK_SET){
		if(iL.inodeArray[i].position+offset>iL.inodeArray[i].fileSize){
			printf("\tFileSystem: lseekFS, no se puede posicionar el puntero fuera del fichero\n");
			return -1;
		}
		if(iL.inodeArray[i].position+offset<0){
			printf("\tFileSystem: lseekFS, offset no puedeser menor que cero\n");
			return -1;
		}
		iL.inodeArray[i].position=offset+iL.inodeArray[i].position;
		printf("FileSystem: lseekFS, se ha movido el puntero de lectura/escritura movido a la posicion %d del fichero correctamente\n",iL.inodeArray[i].position);
		return iL.inodeArray[i].position;
	}
	return -1;
}

/**********************/
/* Version management */
/**********************/

/*
 * Tags a file with the given tag name. Returns 0 if the operation is
 * successful, 1 if the file already had that tag or -1 in case of error.
 */
int tagFS(char *fileName, char *tagName) {
	int i=0, found=0, j=0;
	// Recorremos los inodos buscando el archivo pedido
	while(i<sB.inodeNumber && found==0){
		if(strcmp(iL.inodeArray[i].name,fileName) == 0){
			found = 1;
		}else{
			i++;
		}
	}
	//si el archivo no ha sido encontrado mandamos un mensaje de error
	if(found == 0){
		printf("\tFileSystem: tagFS, el archivo %s no esta en el sistema\n", fileName);
		return -1;
	}
	//si el archivo ya esta abierto mandamos un mensaje de error
	if(iL.inodeArray[i].open == OPENED){
		printf("\tFileSystem: tagFS, el archivo %s  se encuentra abierto\n",fileName);
		return -1;
	}
	//si el archivo ya tiene 3 etiquetas mandamos un mensaje de error
	if(iL.inodeArray[i].indexTag == 3){
		printf("\tFileSystem: tagFS, el archivo %s  ya tiene 3 etiquetas asociadas\n",fileName);
		return -1;
	}
	found = 0;
	//tags[MAX_TAG];
	//buscamos en las etiquetas si ya existe la etiqueta 
	while(j<tL.index && found == 0){
		if(strcmp(tL.tags[j].name,tagName) == 0)
			found=1;
		else
			j++;
	}
	//si existe la etiqueta, comprobamos si el archivo seleccionado ya tiene esta etiqueta asociada
	if(found == 1){
		if(tL.tags[j].indexImap[i] == TAG){
			printf("\tFileSystem: tagFS, el archivo %s  ya esta asociado a la etiqueta %s\n",fileName, tagName);
			return 1;
		}
		else{
			printf("FileSystem: tagFS, el archivo %s  ha sido asociado correctamente a la etiqueta %s\n",fileName, tagName);
			tL.tags[j].indexImap[i] = TAG;
			iL.inodeArray[i].indexTag++;
			tL.tags[j].i++;
			return 0;
		}
	}
	//creamos la nueva etiqueta
	if(tL.index<MAX_TAG){
		strcpy(tL.tags[tL.index].name,tagName);
		tL.tags[tL.index].indexImap[i]=TAG;
		tL.index++;
		iL.inodeArray[i].indexTag++;
		tL.tags[j].i++;
		printf("FileSystem: tagFS, el archivo %s  ha sido asociado correctamente a la etiqueta %s\n",fileName, tagName);
		return 0;
	}
	else{
		printf("\tFileSystem: tagFS, se ha alcanzado el maximo numero de etiquetas en el sistema\n");
		return -1;
	}
	printf("\tFileSystem: tagFS, se ha producido un error\n");
	return -1;
}

/*
 * Removes a tag from a file. Returns 0 if the operation is successful, 1 if
 * the tag wasn't associated to the file or -1 in case of error.
 */
int untagFS(char *fileName, char *tagName) {
	int i=0, found=0, j=0;
	// Recorremos los inodos buscando el archivo pedido
	while(i<sB.inodeNumber && found==0){
		if(strcmp(iL.inodeArray[i].name,fileName) == 0 && sB.iMap[i] == 1){
			found = 1;
		}else{
			i++;
		}
	}
	//si el archivo no ha sido encontrado mandamos un mensaje de error
	if(found == 0){
		printf("\tFileSystem: untagFS, el archivo %s no esta en el sistema\n", fileName);
		return -1;
	}
	//si el archivo ya esta abierto mandamos un mensaje de error
	if(iL.inodeArray[i].open == OPENED){
		printf("\tFileSystem: untagFS, el archivo %s  se encuentra abierto\n",fileName);
		return -1;
	}
	found = 0;
	//tags[MAX_TAG];
	//buscamos en las etiquetas si ya existe la etiqueta 
	while(j<tL.index && found == 0){
		if(strcmp(tL.tags[j].name,tagName) == 0)
			found=1;
		else
			j++;
	}
	if(found == 0){
		printf("\tFileSystem: untagFS, la etiqueta  %s no esta en el sistema\n", tagName);
		return -1;
	}else{
		if(tL.tags[j].indexImap[i] == TAG){
			tL.tags[j].indexImap[i] = UNTAG;
			iL.inodeArray[i].indexTag--;
			tL.tags[j].i--;
			if(tL.tags[j].i == 0){
				//sB.tMap[j]=0;
				tL.tags[j]=tL.tags[tL.index-1];
				tL.index--;
			}
			printf("FileSystem: untagFS, el archivo %s  ha sido desvinculado correctamente de la etiqueta %s\n",fileName, tagName);
			return 0;
		}else{
			printf("\tFileSystem: untagFS, la etiqueta %s no esta asociada al fichero %s\n",fileName,tagName);
			return 1;
		}
	}
	printf("\tFileSystem: untagFS, se ha producido un error\n");
	return -1;
}

/*
 * Looks for all files tagged with the tag tagName, and stores them in the list
 * passed as a parameter. Returns the number of tagged files found or -1 in
 * case of error.
 */
int listFS(char *tagName, char **files) {
	int j=0, found=0, i, k=0;
	while(j<tL.index && found == 0){
		if(strcmp(tL.tags[j].name,tagName) == 0)
			found=1;
		else
			j++;
	}
	if(found == 0){
		printf("\tFileSystem: listFS, la etiqueta  %s no esta en el sistema\n", tagName);
		return -1;
	}else{
		printf("FileSystem: listFS, los siguientes archivos estan asociados a la etiqueta %s :\n", tagName);
		//files = (char**) malloc(MAX_FILES*sizeof(char*));
		for(i=0;i<MAX_FILES;i++){
			if(tL.tags[j].indexImap[i] == TAG){
				//files[k] = (char*) malloc(64*sizeof(char));
				strcpy(files[k],iL.inodeArray[i].name);
				printf("%s\n",iL.inodeArray[i].name);
				k++;
			}
		}
		//printf("PRUEBA %s\n",files[0]);
		
		return k;
	}
	return -1;
}
