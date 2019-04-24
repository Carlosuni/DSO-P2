/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#define BLOCK_SIZE 2048 //bytes
#define MAX_HDD 10490000000L //bytes
#define MIN_HDD 51200L //bytes
#define MAX_FILE_SIZE 2048 //bytes
#define MAX_FILES 40 //unds
#define DIR_MAX_FILES 10 //unds
#define LNG_FILE_DIR_NAME 32 //unds
#define MAX_PATH_FILE 132 //unds, nombre incluido
#define MAX_PATH_FOLDER 99 //unds, nombre incluido


#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

//estructuras de datos y metadatos aqui 
struct i_nodo
{
  char nombre[32];
  char data[BLOCK_SIZE];
  int usado; // 0 no usado y 1 usado
  int number;								/* Numero de bloques que componen el fichero */
  int position;
};


struct SB {
  int numBloquesMapaDatos; 			/* Número de bloques del mapa datos */
  int numInodos; 						/* Número de inodos en el dispositivo */
  int primerInodo; 					/* Número bloque del 1º inodo del disp. (inodo raíz) */
  int numBloquesDatos; 				/* Número de bloques de datos en el disp. */
  int busynodes;						/* Numero de nodos que estan en uso */	
  int primerBloqueDatos; 				/* Número de bloque del 1º bloque de datos */
  int tamDispositivo; 					/* Tamaño total del disp. (en bytes) */
  int numTotalBloques;					/* Numero total de bloques pasador por parametro */
  char bitMap[MAX_FILES/8];			/* Mapa de bits inodos 5B */
  struct i_nodo arraynode [MAX_FILES]; 		/* array de i- nodos 1600B */
};



