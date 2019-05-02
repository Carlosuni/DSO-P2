/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */

#include "include/filesystem.h"		// Headers for the core functionality
#include "include/auxiliary.h"		// Headers for auxiliary functions
#include "include/metadata.h"		// Type and structure declaration of the file system
#include "include/crc.h"			// Headers for the CRC functionality
#include <string.h>					// headers for the strcmp functionality
#include <stdlib.h>

/********************************/
/******** GLOBAL VARIABLES   ****/
/********************************/
// variable so we can use the 'bread' function, it acts as a buffer
char tBlock[MAX_BLOCK_SIZE];
superBlock SB;
/********************************/
/******** AUXILIARY FUNCTIONS****/
/********************************/
/*
 * @brief 	Gets the position of a iNode given its file descriptor.
 * @return 	position of the iNode if success, -1 otherwise.
 */
int getPos(int fd) {

	if (fd < 1) {
		perror("Error: file descriptor not found.");
		return -1;
	}

	/* Search for the position looking for the iNode that has the same fd as the parameter. */
	int i;
	for (i = 0; i < SB.inodeNum; i++) {
		if (SB.inodeArray[i].fileDescriptor == fd) {
			return i;
		}
	}

	/* If we cant find it, then return -1 */
	return -1;
}


/********************************/
/******** FUNCTIONS    s      ****/
/********************************/

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{
	/*
	*	A6: filesystem must be contained in disks between 50 KiB & 100 KiB
	*		50 KiB = 51200 Bytes
	*		100 KiB = 102400 Bytes
	*/
	if (deviceSize >= 102401 || deviceSize <= 51199)
	{
		perror("Error : filesystem must be contained in disks between 50 KiB & 100 KiB");
		return -1;
	}else{
		// generate SB values
		// integer rounding up, so if there is a need of "half" a block, it will create one more
		SB.inodeNum = (deviceSize + (2048 - 1))/ 2048 -1;
		// generate the filesize, if the number of nodes is higher than the 
		// specified deviceSize (usually in one more) it will allocate the space necesary
		SB.size = SB.inodeNum * 2048 + 2048;
		// put to zeroes the inodeMap
		int i =  0;
		for (i = 0; i < SB.inodeNum; ++i)
		{
			SB.inodeMap[i] = 0;
			// set to default the inode array
			SB.inodeArray[i].crc = 0; // the initial crc always is 0, so we don't waste time calling the funtcion
			SB.inodeArray[i].fileSize = 0;
			SB.inodeArray[i].fileDescriptor = 0;
			SB.inodeArray[i].filePointer = 0;
			SB.inodeArray[i].open = 0;
		}
		unmountFS();
		
	}
	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	return 0;
	// check the filesystem before mount, so we know for sure it's all ok
	int error=0;
/////////////////////////////////////////////
	// create a  var position to store the inode position we're checking
	uint8_t position = 0;
	uint8_t crcValue = 1;
	char * buffer = "";

	// check all the files in the FS
	for (position = 1; position < SB.inodeNum; ++position)
	{
		// if the block it's in usem we check it
		if (SB.inodeMap[position] == 1)
		{			
			uint8_t bread_error = bread(DEVICE_IMAGE, SB.inodeArray[position].fileDescriptor, buffer);
			if (bread_error == -1)
			{
				perror("bread: error, could not read the block.");
				error= -2;
			}
			crcValue = CRC16((unsigned char *)buffer, sizeof(&buffer));
			if (crcValue < 0)
			{
				perror("check: CRC16 has failed.");
				error= -2;
			}
			if (SB.inodeArray[position].crc == crcValue)
			{
				printf("check: check passed for the file %s\n", SB.inodeArray[position].name);
			}else{
				perror("check: file corrupted");
				error= -1;
			}
		}
	}
		/////
	if(error != 0)
	{
		perror("mountFS: could not mount the filesystem because it's corrupted.");
		return -1;
	}
	// Leer el primer bloque, donde se situan los metadatos
	error = bread(DEVICE_IMAGE, 0, (char*) &SB);

	if(error == 0){
		return error;
	}
	if(error == -1)
	{
		perror("Error al montar FS.");
	}
	return error;	
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	return 0;
	// Escribe los datos del buffer en el device_image
	int error = bwrite(DEVICE_IMAGE, 0, (char*) &SB);

	if(error == 0){
		return 0;
	}
	if(error == -1)
	{
		perror("Error al desmontar");
	}
	return -1;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system..
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */

int createFile(char *path)
{
	// newPosition holds the info about which position is free
	uint8_t newPosition = -1;

	/*
	*	ERRORS CHECK
	*/
	// Check if there is space available and if the file already exists
	for (newPosition = 0; newPosition < SB.inodeNum; newPosition++) 
	{
		if (strcmp(SB.inodeArray[newPosition].name, path) == 0 && SB.inodeMap[newPosition] == 1) {
			perror("createFile: a file with the same name already exists");
			return -1;
		}
		// if there's a free position on the inodeMap, it exits the loop
		if(SB.inodeMap[newPosition] == 0) 
		{ 
			break; 
		}
	}
	// if newPosition equals the maximum number of files, it indicates there's no 
	// space left (if maximum is 49 files, it counts from 0 to 48 so if newPosition == 49, it 
	// has reached the end without finding a space)
	if (newPosition == SB.inodeNum) {
		perror("CreateFIle: Maximum number of files reached.");
		return -2;
	}
	// ERRORS PASSED

	// creation of the file
	// set the name of the file
	strcpy(SB.inodeArray[newPosition].name, path);	
	// set the file as closed
	SB.inodeArray[newPosition].open = 0; 
	
	// set the position in the fileDescriptor == block number +1(block of metadata)
	SB.inodeArray[newPosition].fileDescriptor = newPosition +1;
	// set the initial size
	SB.inodeArray[newPosition].fileSize = 0;

	// set the file pointer
	SB.inodeArray[newPosition].filePointer = 0;	
	// mark the inodeMap with a '1'
	SB.inodeMap[newPosition] = 1;

	bwrite(DEVICE_IMAGE, SB.inodeArray[newPosition].fileDescriptor, (char*) &tBlock);

	// check file integrity
	char bread_buffer[2048];
	uint8_t bread_error = bread(DEVICE_IMAGE, SB.inodeArray[newPosition].fileDescriptor, (char *)bread_buffer);
	if (bread_error == -1)
	{
		perror("bread: error, could not read the block.");
		return -2;
	}
	SB.inodeArray[newPosition].crc = CRC16((unsigned char *)bread_buffer, (int)sizeof(bread_buffer));

	return 0;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error. 
 */
int removeFile(char *path)
{
	// we search for the block position in the inode array wich has the same name as the path
	uint8_t position = 0;
	for (position = 0; position < SB.inodeNum; ++position)
	{
		// if the inode.name and the path are the same, we obliterate the block
		if (strcmp(SB.inodeArray[position].name, path) == 0) {
			if (SB.inodeMap[position] == 0)
			{
				perror("removeFile: this cannot happen");
				return -2;
			}
			if (SB.inodeArray[position].open == 1)
			{
				perror("removeFile: the file is open, it can't be removed.");
				return -2;
			}
			// reinitialize to -1 all the atributes
			SB.inodeArray[position].crc = -1;
			SB.inodeArray[position].fileSize = -1;
			SB.inodeArray[position].fileDescriptor = -1;
			SB.inodeArray[position].filePointer = -1;
			memset(&(SB.inodeArray[position].name), 0, 32);
			SB.inodeArray[position].open = -1;
			SB.inodeMap[position] = 0;
			return 0;
		}
	}
	perror("removeFile: file not found.");
	return -1;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *path)
{
	// check the file before open, so we know for sure if it is ok

	////
	int error = 0;

	uint8_t position = 0;
	uint8_t crcValue = 1;
	char * buffer = "";
	// search for the file in the filesystem
	for (position = 1; position < SB.inodeNum; ++position)
	{
		// obliterate the block
		if (strcmp(SB.inodeArray[position].name, path) == 0) {
			uint8_t bread_error = bread(DEVICE_IMAGE, SB.inodeArray[position].fileDescriptor, buffer);
			if (bread_error == -1)
			{
				printf("fileDescriptor: %d\n", SB.inodeArray[position].fileDescriptor);	
				perror("bread: error, could not read the block.");
				return -2;
			}
			crcValue = CRC16((unsigned char *)buffer, sizeof(&buffer));
			printf("crcValue: %d\n", crcValue);
			if (crcValue < 0)
			{
				perror("checkFS: CRC16 has failed.");
				return -2;
			}
			if (SB.inodeArray[position].crc == crcValue)
			{
				printf("checkFile: check passed\n");
				return 0;
			}else{
				perror("checkFile: file corrupted");
				return -1;
			}
		}
	}
	////
	if(error == -1)
	{
		perror("openFile: could not open the file because it's corrupted.");
		return -2;
	}
	int encontrado= 0;
	int aux;

	for (aux = 0; aux < SB.inodeNum; aux++) {
		if (strcmp(SB.inodeArray[aux].name, path) == 0 && SB.inodeMap[aux] == 1){
			encontrado = 1;
			break;
		}
	}
	// Si no lo hemos encontrado
	if(encontrado == 0){
		perror("Fichero no encontrado \n");
		return -1;
	}
	// Si el archivo ya esta abierto, enviamos un aviso
	if(SB.inodeArray[aux].open == 1){
		perror("El fichero ya esta abierto \n");
		return -2;
	}

	//Vamos a abrir el fichero
	SB.inodeArray[aux].open = 1; //indicamos que esta abierto
	SB.inodeArray[aux].filePointer = 0; //puntero al inicio del documento
	SB.inodeArray[aux].fileDescriptor = aux + 1;

	return SB.inodeArray[aux].fileDescriptor;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	int posicion = getPos(fileDescriptor);
	if(posicion == -1){
		perror("fileDescriptor no encontrado en la funcion closeFile \n");
		return -1;
	}
	//Vamos a cerrar el fichero
	SB.inodeArray[posicion].open = 0; //indicamos que esta cerrado	
	return 0;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{

	/* Retrieve file position given its fd */
	int position = getPos(fileDescriptor);

	/* If we cant find it, return -1 */
	if (position == -1) {
		return -1;
	}
	// check if the file is open
	if (SB.inodeArray[position].open == 0)
	{
		perror("readFile: error, the file is closed");
		return -1;
	}
	/* Number of bytes to be read has to be at least 1*/
	if (numBytes <= 0) {
		return -1;
	}

	/* If the offset is set on the EOF, return 0 (we read 0 bytes) */
	if (SB.inodeArray[position].filePointer == SB.inodeArray[position].fileSize) {
		return 0;
	}

	/* How many bytes are we going to read ? */
	if (SB.inodeArray[position].filePointer + numBytes > SB.inodeArray[position].fileSize){
		numBytes = SB.inodeArray[position].fileSize - SB.inodeArray[position].filePointer;
	}

	/* We read on the fileDescriptor block_number. */
	bread(DEVICE_IMAGE, SB.inodeArray[position].fileDescriptor, buffer);

	/* Set the offset to the current position */
	SB.inodeArray[position].filePointer += numBytes;
	return numBytes;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	/* Retrieve file position given its fd */
	int position = getPos(fileDescriptor);

	/* If we cant find it, return -1 */
	if (position == -1) {
		perror("writeFile: file not found.");
		return -1;
	}
	// check if the file is open
	if (SB.inodeArray[position].open == 0)
	{
		perror("writeFile: error, the file is closed");
		return -1;
	}
	/* Number of bytes to be read has to be at least 1*/
	if (numBytes <= 0) {
		return -1;
	}

	/* If we try to write at the EOF, return 0 (no bytes could be written) */
	if (SB.inodeArray[position].filePointer == MAX_BLOCK_SIZE) {
		return 0;
	}

	/* How many bytes are we going to write? */
	if (SB.inodeArray[position].filePointer + numBytes > MAX_BLOCK_SIZE){
		numBytes = MAX_BLOCK_SIZE - SB.inodeArray[position].filePointer;
	}

	/* Get the block 
	char auxiliary[MAX_BLOCK_SIZE];
  int i;
  for (i = 0; i < MAX_BLOCK_SIZE; i++) { 
		auxiliary[i] = '0';
	}*/
 
	char content[2048];
	memset(content, 0, 2048);
	strcpy(content, buffer);
	bwrite(DEVICE_IMAGE, SB.inodeArray[position].fileDescriptor, content);

	// check file integrity
	char bread_buffer[2048];
	uint8_t bread_error = bread(DEVICE_IMAGE, SB.inodeArray[position].fileDescriptor, (char *)bread_buffer);
	if (bread_error == -1)
	{
		perror("bread: error, could not read the block.");
		return -1;
	}
	SB.inodeArray[position].crc = CRC16((unsigned char *)bread_buffer, (int)sizeof(bread_buffer))	;


	/* Set the offset to the current position */
	SB.inodeArray[position].filePointer += sizeof(buffer);

	/* Get the number of bytes in the file 
	for (i = 0; i < MAX_BLOCK_SIZE; i++) { 
		if (auxiliary2[i] == 0 && auxiliary2[i-1] != 0){
			SB.inodeArray[position].fileSize = i;
		} 
	}*/
	SB.inodeArray[position].fileSize = sizeof(buffer);
	return sizeof(buffer);
}



/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	/* Retrieve file position given its fd */
	int position = getPos(fileDescriptor);

	/* If we cant find it, return -1 */
	if (position == -1) {
		return -1;
	}

	/* If whence is FS_SEEK_END, set the offset at the end of the file*/
	if (whence == FS_SEEK_END) {
		SB.inodeArray[position].filePointer = SB.inodeArray[position].fileSize;
		return 0;
	}
	
	/* If whence is FS_SEEK_BEGIN, set the offset at the start of the file */
	if (whence == FS_SEEK_BEGIN) {
		SB.inodeArray[position].filePointer = 0;
		return 0;
	}

	// Si es FS_SEEK_SET, entonces hay que empezar desde el principio del fichero
	/* If whence is FS_SEEK_CUR, then we add offset to the pointer */
	if (whence == FS_SEEK_CUR) {
		if (offset < 0) {
			return -1;
		}else if (offset > SB.inodeArray[position].fileSize) {
			return -1;
		}else{
			SB.inodeArray[position].filePointer = offset;
			return 0;
		}
		
	}

	/* Else, return -1 */
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
