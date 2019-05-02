/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */
#include <stdint.h>
/*
*	GLOBAL VARIABLES
*/
/*
*	A1: max number of files in filesystem mus be less or equal to 40
*/
#define MAX_FILES 40
/*
*	A2: max name longitude must be less or equal than 32 characters
*/
#define MAX_CHAR_NAME 32
/*
*	A4: max block size must be less or equal than 2048 bytes
*/
#define MAX_BLOCK_SIZE 2048
/*
*	STRUCTURES
*/

// inode structure
typedef struct {
	uint16_t crc;
	uint16_t fileSize;
	uint8_t fileDescriptor; 
	uint16_t filePointer; 
	char name[MAX_CHAR_NAME];
	uint8_t open;
} inode;

// super block structure
typedef struct {
	uint8_t inodeNum; // inode number == # of files
	uint16_t size;
	uint8_t inodeMap[49]; // inode map (only contains ones and zeroes), if a position equals 1, the block is used
							// if it equals 0, it's not used
	inode inodeArray[49];

} superBlock;
