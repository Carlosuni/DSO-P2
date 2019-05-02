/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#define BLOCK_SIZE 2048 //bytes
#define MAX_HDD 10485760 //bytes
#define MIN_HDD 51200 // bytes
#define MAX_BLOQUES 5120 //bytes
#define MIN_BLOQUES 25 //bytes
#define MAX_FILE_SIZE 2048 //bytes
#define MAX_FILES 40 //unds
#define DIR_MAX_FILES 10 //unds
#define LNG_FILE_DIR_NAME 32 //unds
#define MAX_PATH_FILE 132 //unds, nombre incluido
#define MAX_PATH_FOLDER 99 //unds, nombre incluido
#define PADDING_SB 0 //unds, nombre incluido
#define PADDING_INODO 0 //unds, nombre incluido
#define T_FILE 0
#define T_DIR 1


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
  int lleno;
};

/* Estructura de inodos */
typedef struct {
  char tipo;                             /* 0=T_FILE o 1=T_DIR */
  char nombre[MAX_PATH_FILE];                     /* Nombre del fichero o directorio asociado */
  unsigned int num_bloque_inodo;         /* Numero del bloque del inodo */
  unsigned int bloque_next_inodo;
  unsigned int inodosContenidos[DIR_MAX_FILES];   /* Si tipo dir, num de files o dirs contenidos, si tipo file, 0 */
  unsigned int num_contenidos;
  unsigned int tamanyo;                 /* Tamanyo actual del fichero en bytes */
  unsigned int bloqueDirecto;           /* Numero del bloque directo */
  unsigned int bloqueIndirecto;         /* Numero del bloque indirecto */
  char relleno[PADDING_INODO];          /* Caracter de relleno = '0' */
  unsigned int en_uso;                   /* Sin usar 0, usado 1*/
  int fd;                   /* Sin usar 0, usado 1*/
} TipoInodoDisco;


/* Estructura de bloques de 2048B*/
typedef struct {
  unsigned int num_magico;                /* Num magico del superbloque: 0x000D5500 */
  unsigned int num_bloques_mapa_inodos;   /* Num de bloques del mapa inodos */
  unsigned int num_bloques_mapa_datos;    /* Num de bloques del mapa datos */
  unsigned int num_inodos;                /* Num de inodos en el dispositivo */
  unsigned int primer_inodo;              /* Num de bloque del primero inodo del disp (raiz) */
  float num_bloque_datos;          /* Num bloques de datos del disp */
  unsigned int num_tot_bloques;          /* Num bloques de datos del disp */
  unsigned int primer_bloque_datos;       /* Num de bloque del primer bloque de datos */
  unsigned int tam_dispositivo;           /* Tamanyo del dispositivo */
  //char relleno[PADDING_SB];               /* Caracter de relleno = '0' */
  //TipoInodoDisco arraynode[MAX_FILES]; 		/* array de i- nodos 1600B */
  int bloques_en_uso;
  unsigned int max_inodos;
} TipoSuperbloque;

typedef struct 
{
  //int num_bloques;								/* Numero de bloques que componen el fichero */
  int pos_actual_bloq;							/* # del bloque actual */
  int siguiente_bloq;								/* Siquiente bloque del fichero */
  char lleno;									/* Booleano 1 o 0 de bloque lleno o vacio, si se escribe 1B en el array ya se cambia a lleno */
  //int fin_buff_byte; 							/* indica hasta que bytes del buffer hay datos */

  char data[2048];							/* Buffer de los datos del fichero */
}  Bloque_datos;


