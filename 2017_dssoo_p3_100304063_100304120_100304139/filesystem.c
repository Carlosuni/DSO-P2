/* This file contains the definition of the functions that must be implemented
 * to allow the user access to the file system and the files.
 */

#include "include/filesystem.h"
#include <string.h>
#include <stdlib.h>
#include "include/snapshot.c"

// Constantes de padding
#define SB_PADDING 3680
#define INODE_PADDING 96

// Constantes de comienzo/fin de metadatos
#define METADATA_BLOCKS 12
#define STARTING_SNAPSHOT_BLOCK 2

// Constante de número de ficheros
#define MAX_FILES 50

// Constantes de apertura/cerrado
#define FILE_CLOSED '0'
#define FILE_OPENED '1'

char dummyBlock[BLOCK_SIZE];

// Definición de la estructura superbloque
typedef struct { 
	unsigned int inodeNumber; // Número inodos
	unsigned int deviceSize; // tam disp
	int iMap[MAX_FILES]; // Mapa de inodos (0 = no creado, 1 = creado)
	SList snapshots; // Lista de snapshots
	char padding[SB_PADDING]; // padding
} SuperBlock;

// Definición de la estructura del inodo
typedef struct { // (80B)
	unsigned int fileSize; /* Tamaño actual del fichero en bytes */
	unsigned int fileDesc; // Descriptor de fichero
	unsigned int pointer; // Puntero de escritura/lectura
	char name[64]; /* Nombre del fichero asociado */
	char isOpen; // Indica si está abierto o no
	char blockNumber; /* Número del bloque directo */
} inode;

// Definiciaón de la lista de i-nodos
typedef struct {
	inode inodeArray[MAX_FILES];
	char padding[INODE_PADDING];
} inodeList;



SuperBlock SB;
inodeList inodes;

/***************************/
/* Funciones auxiliares.   */
/***************************/

// Devuelve la posición de un descriptor de fichero
int getPosition(int fileDesc) {
	int found = 0;
	int aux;

	if (fileDesc < 1) {
		printf("FILESYSTEM: getPosition error -- Invalid file descriptor. Must be higher or equal to 1.");
		return -1;
	}

	// Buscar el fichero correspondiente en los inodos...
	for (aux = 0; aux < SB.inodeNumber; aux++) {
		if (inodes.inodeArray[aux].fileDesc == fileDesc) { found = 1; break; }
		
	}

	// Si no está, enviamos un mensaje de error.
	if (!found) return -1;
	else return aux;
}

// Imprime valores del superbloque
void print() {
	int i;
	printf("SB:\n inodeNumber: %d\n", SB.inodeNumber);
	printf("deviceSize: %d\n", SB.deviceSize);
	printf("iMap:\n { ");
	for (i = 0; i < MAX_FILES; i++) {
		printf("%d ", SB.iMap[i]);
	}
	printf("}\n");
	
	
}



/***************************/
/* File system management. */
/***************************/


/*
 * Formats a device.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mkFS(int maxNumFiles, long deviceSize) {
	// Error si hay demasiados ficheros
	if(maxNumFiles > MAX_FILES) {
		printf("FILESYSTEM: mkFS error -- Maximum number of files cannot be higher than 50.\n");
		return -1;
	}

	// Error si no hay al menos un fichero
	if (maxNumFiles < 1) {
		printf("FILESYSTEM: mkFS error -- Invalid number of files.\n");
		return -1;
	}

	// Error si el dispositivo ocupa menos de 320KB o más de 500KB
	if(deviceSize < 327680 || deviceSize > 512000) {
		printf("FILESYSTEM: mkFS error -- This file system is suited for devices with a size between 320KB and 500KB.\n");
		return -1;
	}

	// Crear el superbloque con sus valores por defecto
	SB.inodeNumber = maxNumFiles;
	SB.deviceSize = deviceSize;

	int aux;
	for (aux=0; aux<50; aux++)
		SB.iMap[aux] = 0;

	SB.snapshots.size = 0;
	SB.snapshots.pointer = 0;

	// Reiniciar los inodos
	for (aux = 0; aux < SB.inodeNumber; aux++) {
		memset(&(inodes.inodeArray[aux]), 0, sizeof(inode) ); 
	}
		
	// Invocar a umount
	umountFS();

	printf("FILESYSTEM: mkFS -- Format completed.\n");
	// print();
	return 0;
}

/*
 * Mounts a file system from the device deviceName.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mountFS() {
	// Leer el superbloque
	bread(DEVICE_IMAGE, 0, (char*) &SB);

	// Leer el estado de los nodos
	bread(DEVICE_IMAGE, 1, (char*) &inodes);

	// print();
	printf("FILESYSTEM: mountFS OK.\n");
	return 0;
}

/*
 * Unmount file system.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int umountFS() {
	// Colocar el superbloque en la posición 0.
	bwrite(DEVICE_IMAGE, 0, (char*) &SB);

	// Colocar los inodos en la posición 1.
	bwrite(DEVICE_IMAGE, 1, (char*) &inodes);


	printf("FILESYSTEM: umountFS OK.\n");

	return 0;
}

/*
 * Undo previous change in the file system. A change can be either a write
 * operation on an existing file, or the creation of a new file. Calling this
 * function when there are no more changes to be undone is considered an error.
 * Returns the number of remaining changes that can be undone if the operation
 * was correct or -1 in case of error.
 */
int undoFS() {
	int x, openedFiles = 0;

	// Si no hay cambios posibles
	if (SB.snapshots.size == 0) {
		printf("FILESYSTEM: undoFS error -- There are no recent changes to be reverted.\n");
		return -1;
	}

	// Si hay ficheros abiertos
	for (x=0; x<SB.inodeNumber; x++) {
		if (SB.iMap[x] && inodes.inodeArray[x].isOpen == FILE_OPENED) openedFiles++;
	}

	if (openedFiles != 0) {
		printf("FILESYSTEM: undoFS error -- All files must be closed before performing an undo operation. There are currently %d file(s) opened.\n", openedFiles);
		return -1;
	}

	// Obtener datos del siguiente snapshot
	snapshot *aux = malloc (sizeof(snapshot));
	if (delete(&SB.snapshots, aux) == -1) return -1;

	// Si la operación previa fue abrir un fichero...
	if (aux->operation == CREATE_FILE) {
		SB.iMap[aux->affected] = 0;
		printf("FILESYSTEM: undoFS -- %s has been deleted.\n",  inodes.inodeArray[aux->affected].name);
		//print();
		return SB.snapshots.size;
	}
	
	// Si la operación previa fue de escritura...
	if (aux->operation == WRITE_FILE) {
		// Recuperar al bloque que referencia el snapshot
		char backup [BLOCK_SIZE];
		bread(DEVICE_IMAGE, aux->block, (char*) &backup);
		// Escribir en el bloque del inodo afectado
		bwrite(DEVICE_IMAGE, inodes.inodeArray[aux->affected].blockNumber, (char*) &backup);

		// Restablecer puntero y tamaño de fichero anteriores
		inodes.inodeArray[aux->affected].pointer = aux->prevPointer;
		inodes.inodeArray[aux->affected].fileSize = aux->fileSize;

		printf("FILESYSTEM: undoFS -- reverted write operation on file \"%s\".\n", inodes.inodeArray[aux->affected].name);
		return SB.snapshots.size;
	}

	printf("FILESYSTEM: undoFS error -- Fatal exception in snapshot system.\n");
	return -1;
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
	//print();
	int aux, newFilePos;
	int found = 0;

	// Verificar si hay espacio para un nuevo fichero
	for (newFilePos = 0; newFilePos < SB.inodeNumber; newFilePos++) {
		if(!SB.iMap[newFilePos]) { found = 1; break; }
	}

	if (!found) {
		printf("FILESYSTEM: creatFS error -- No space for new file. There are already %d files created.\n", SB.inodeNumber);
		return -1;
	}

	// Verificar si ya existe el fichero
	for (aux = 0; aux < SB.inodeNumber; aux++) {
		if (strcmp(inodes.inodeArray[aux].name, fileName) == 0 && SB.iMap[aux]) {
			printf("%d\n", SB.iMap[newFilePos]);
			printf("FILESYSTEM: creatFS note -- File %s was already created.\n", fileName);
			return 1;
		}
	}

	// Si no, se procede a crear el fichero
	strcpy(inodes.inodeArray[newFilePos].name, fileName);
	inodes.inodeArray[newFilePos].isOpen = FILE_CLOSED;
	inodes.inodeArray[newFilePos].blockNumber = METADATA_BLOCKS+newFilePos; // 12 de metadatos
	bwrite(DEVICE_IMAGE, inodes.inodeArray[newFilePos].blockNumber, (char*) &dummyBlock); // Reiniciar bloque
	inodes.inodeArray[newFilePos].fileSize = 0;
	inodes.inodeArray[newFilePos].pointer = 0;
	inodes.inodeArray[newFilePos].fileDesc = 0;
	
	// Poner 1 en el mapa de inodos
	SB.iMap[newFilePos] = 1;
	//print();


	printf("FILESYSTEM: creatFS -- Successfully created file \"%s\".\n", inodes.inodeArray[newFilePos].name);

	// Se crea un snapshot de este hecho
	snapshot newSnap = {CREATE_FILE, newFilePos, 0, 0, 0};
	SB.snapshots = add(SB.snapshots, newSnap);

	return 0;
}

/*
 * Opens an existing file.
 * Returns file descriptor if possible, -1 if file does not exist or -1 in case
 * of error.
 */
int openFS(char *fileName) {

	int found = 0;
	int aux;

	// Buscar el fichero correspondiente en los inodos...
	for (aux = 0; aux < SB.inodeNumber; aux++) {
		if (SB.iMap[aux] == 1 && strcmp(inodes.inodeArray[aux].name, fileName) == 0) { found = 1; break;}
	}

	// Si no está, enviamos un mensaje de error.
	if (!found) {
		printf("FILESYSTEM: openFS error -- File \"%s\" was not found.\n", fileName);
		return -1;
	}

	// Si encontramos el fichero, lo abrimos. Si ya está abierto, emitimos un mensaje de aviso
	if (inodes.inodeArray[aux].isOpen == FILE_OPENED) {
		printf("FILESYSTEM: openFS note -- File \"%s\" is already opened.\n", fileName);
		return 0;
	}

	// Ponemos el fichero en estado de abierto, y devolvemos aux.
	inodes.inodeArray[aux].pointer = 0;
	inodes.inodeArray[aux].isOpen = FILE_OPENED;
	inodes.inodeArray[aux].fileDesc = aux+1; // Los descriptores empiezan desde 1.
	printf("FILESYSTEM: openFS -- File %s was opened (descriptor: %d).\n", inodes.inodeArray[aux].name, inodes.inodeArray[aux].fileDesc);

	return inodes.inodeArray[aux].fileDesc;
}

/*
 * Closes a file.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int closeFS(int fileDescriptor) {

	int position = getPosition(fileDescriptor);

	// Si no está, enviamos un mensaje de error.
	if (position == -1) {
		printf("FILESYSTEM: closeFS error -- File descriptor %d was not found.\n", fileDescriptor);
		return -1;
	}


	printf("FILESYSTEM: closeFS -- Closing file %s (%d)...\n", inodes.inodeArray[position].name, fileDescriptor);
	// Ponemos el fichero en estado de cerrado.
	inodes.inodeArray[position].isOpen = FILE_CLOSED;
	inodes.inodeArray[position].fileDesc = 0; // En 0, nunca se identificará su descriptor.

	return 0;
}

/*
 * Reads a number of bytes from a file and stores them in a buffer.
 * Returns the number of bytes read or -1 in case of error.
 */
int readFS(int fileDescriptor, void *buffer, int numBytes) {
	
	// Buscar el fichero a partir de su descriptor
	int position = getPosition(fileDescriptor);


	// Si no está, enviamos un mensaje de error.
	if (position == -1) {
		printf("FILESYSTEM: readFS error -- File descriptor %d was not found.\n", fileDescriptor);
		return -1;
	}

	
	// Emitir error si el número de bytes a leer es 0 ó menos.
	if (numBytes <= 0) {
		printf("FILESYSTEM: readFS error -- numBytes must be higher than zero.\n");
		return -1;
	}

	// Devolver 0 si se lee cuando se ha llegado al final del fichero.
	if (inodes.inodeArray[position].pointer == inodes.inodeArray[position].fileSize) {
		printf("FILESYSTEM: readFS note -- Pointer is at end of file. Nothing to read.\n");
		return 0;
	}

	// Calculamos cúantos bytes vamos a leer.
	if (inodes.inodeArray[position].pointer + numBytes > inodes.inodeArray[position].fileSize)
		 numBytes = inodes.inodeArray[position].fileSize - inodes.inodeArray[position].pointer;

	// Realizamos la lectura
	char block[BLOCK_SIZE];
	bread(DEVICE_IMAGE, inodes.inodeArray[position].blockNumber, block);

	// Pasamos los datos
	memmove(buffer, block+inodes.inodeArray[position].pointer, numBytes);
	inodes.inodeArray[position].pointer += numBytes;
	printf("FILESYSTEM: readFS -- Successfully read %d bytes from file \"%s\" (%d).\n", numBytes, inodes.inodeArray[position].name, inodes.inodeArray[position].fileDesc);

	return numBytes;
}

/*
 * Reads number of bytes from a buffer and writes them in a file.
 * Returns the number of bytes written, 0 in case of end of file or -1 in case
 * of error.
 */
int writeFS(int fileDescriptor, void *buffer, int numBytes) {
	
	// Buscar el fichero a partir de su descriptor
	int position = getPosition(fileDescriptor);

	// Si no está, enviamos un mensaje de error.
	if (position == -1) {
		printf("FILESYSTEM: writeFS error -- File descriptor %d was not found.\n", fileDescriptor);
		return -1;
	}

	
	// Emitir error si el número de bytes a escribir es 0 ó menos.
	if (numBytes <= 0) {
		printf("FILESYSTEM: writeFS error -- numBytes must be higher than zero.\n");
		return -1;
	}

	// Devolver 0 si se lee cuando se ha llegado al final del fichero.
	if (inodes.inodeArray[position].pointer == BLOCK_SIZE) {
		printf("FILESYSTEM: readFS note -- Pointer is at position %d. Operation canceled.\n", BLOCK_SIZE);
		return 0;
	}


	// Procesamiento del snapshot. Mandamos el bloque a una posición de snapshot libre (definido por el pointer)
	char backup[BLOCK_SIZE];
	// printf("CHECK SNAPSHOT BLOCK: %d\n", STARTING_SNAPSHOT_BLOCK+(SB.snapshots.pointer));
	bread(DEVICE_IMAGE, inodes.inodeArray[position].blockNumber, (char*) &backup);
	bwrite(DEVICE_IMAGE, STARTING_SNAPSHOT_BLOCK+(SB.snapshots.pointer), (char*) &backup);

	// El snapshot contiene:
	// 1. Operación de escritura
	// 2. i-nodo afectado
	// 3. Bloque en donde se guardarán los datos previos
	// 4. Tamaño original del fichero antes de la escritura
	// 5. Puntero anterior
	snapshot newSnap = {WRITE_FILE, position, SB.snapshots.pointer+STARTING_SNAPSHOT_BLOCK, inodes.inodeArray[position].fileSize, inodes.inodeArray[position].pointer};

	SB.snapshots = add(SB.snapshots, newSnap);


	// Calculamos cúantos bytes vamos a escribir.
	if (inodes.inodeArray[position].pointer + numBytes > BLOCK_SIZE)
		numBytes = BLOCK_SIZE - inodes.inodeArray[position].pointer;

	// Localizamos el bloque y la posición en donde vamos a escribir.
	char block[BLOCK_SIZE];
	bread(DEVICE_IMAGE, inodes.inodeArray[position].blockNumber, block);

	// Realizamos la escritura
	memmove(block+inodes.inodeArray[position].pointer, buffer, numBytes);
	bwrite(DEVICE_IMAGE, inodes.inodeArray[position].blockNumber, block);

	inodes.inodeArray[position].pointer += numBytes;

	// Calcular cuántos bytes hay ahora en el fichero.
	int aux;
	for (aux = 0; aux < BLOCK_SIZE; aux++) { if (block[aux] == 0) break; }

	inodes.inodeArray[position].fileSize = aux;
	printf("FILESYSTEM: writeFS -- Successfully written %d bytes to file \"%s\" (%d).\n", numBytes, inodes.inodeArray[position].name, inodes.inodeArray[position].fileDesc);
	return numBytes;
}


/*
 * Repositions the pointer of a file. A greater offset than the current size, or
 * an offset lower than 0 are considered errors.
 * Returns new position or -1 in case of error.
 */
int lseekFS(int fileDescriptor, long offset, int whence) {
	// Primero habría que comprobar si el descriptor de fichero se corresponde con un fichero abierto...
	int position = getPosition(fileDescriptor);

	// Si no está, enviamos un mensaje de error.
	if (position == -1) {
		printf("FILESYSTEM: lseekFS error -- File descriptor %d was not found.\n", fileDescriptor);
		return -1;
	}

	// Comprobar si whence es FS_SEEK_END. En este caso se sitúa el puntero al final del fichero ignorando
	// el segundo parámetro
	if (whence == FS_SEEK_END) {
		inodes.inodeArray[position].pointer = inodes.inodeArray[position].fileSize;
		printf("FILESYSTEM: lseekFS -- Pointer sucessfully moved to end of file.\n");
		return 0;
	}
	
	// Si es FS_SEEK_SET, entonces hay que empezar desde el principio del fichero
	if (whence == FS_SEEK_SET) {
		if (offset < 0) {
			printf("FILESYSTEM: lseekFS error -- offset cannot be lower than zero in a FS_SEEK_SET operation.\n");
			return -1;
		}
		if (offset > inodes.inodeArray[position].fileSize) {
		 	printf("FILESYSTEM: lseekFS error -- pointer cannot be placed outside of the file.\n");
		 	return -1;
		}
		inodes.inodeArray[position].pointer = offset;
		printf("FILESYSTEM: lseekFS -- Pointer sucessfully moved to position %ld/%d.\n", offset, inodes.inodeArray[position].fileSize);
		return 0;
	}

	// Si whence no corresponde a ninguno de los dos...
	printf("FILESYSTEM: lseekFS error -- invalid whence value (typo?).\n");
	return -1;
}

