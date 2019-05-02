/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#define BLOCK_SIZE 2048 //bytes
#define MAX_HDD 10485760L //bytes
#define MIN_HDD 51200L // bytes
#define MAX_FILE_SIZE 2048 //bytes
#define MAX_FILES 40 //unds
#define DIR_MAX_FILES 10 //unds
#define LNG_FILE_DIR_NAME 32 //unds
#define MAX_PATH_FILE 132 //unds, nombre incluido
#define MAX_PATH_FOLDER 99 //unds, nombre incluido
#define PADDING_SB '0' //unds, nombre incluido
#define PADDING_INODO '0' //unds, nombre incluido
#define DIVISOR '/'
//#define TAM_INODO sizeof(struct i_nodo)
//#define TAM_SB sizeof(struct SB)


#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

char * bmap; //mapa de bloques, 0 no usado 1 si
char * inodos; //mapa de inodos, 0 no usado 1 si

//estructuras de datos y metadatos aqui 
struct i_nodo
{
  unsigned short id;
  char nombre[32];
  unsigned short bloque_datos; //referencia al bloque de datos si es un archivo
  unsigned short isDir; //indica si es directorio, 1 si, 0 no
  unsigned short archivos[10]; //referencia de inodos que posee el directorio
  unsigned short usado; // 1 si, 0 no
};


struct SB {
  unsigned int num_magico;             /* Numero magico */
  unsigned short tam_dispositivo; 				/* Tamaño total del disp. (en bytes) */
  unsigned short num_inodos; 						/* Número de inodos en el dispositivo */
  unsigned short num_bloques_Datos;      /* Numeros de bloques de datos */
  unsigned short numero_bloques_totales; /* Numero de bloques totales */
  unsigned short numero_bloques_inodos; /* Numero de bloques totales */
};




/* Estructura de inodos */
typedef struct {
  unsigned int tipo;                    /* T_FILE o T_DIR */
  char nombre[200];                     /* Nombre del fichero o directorio asociado */
  unsigned int inodosContenidos[200];   /* Si tipo dir, num de files o dirs contenidos, si tipo file, 0 */
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
  unsigned int num_bloque_datos;          /* Num bloques de datos del disp */
  unsigned int num_tot_bloques;          /* Num bloques de datos del disp */
  unsigned int primer_bloque_datos;       /* Num de bloque del primer bloque de datos */
  unsigned int tam_dispositivo;           /* Tamanyo del dispositivo */
  char relleno[PADDING_SB];               /* Caracter de relleno = '0' */
  char ibitMap[MAX_FILES/8];			/* Mapa de bits inodos 5B */
  //TipoInodoDisco arraynode[MAX_FILES]; 		/* array de i- nodos 1600B */
  int bloques_en_uso;
} TipoSuperbloque;

typedef struct 
{
  int num_bloques;								/* Numero de bloques que componen el fichero */
  int pos_bloq_actual;							/* # del bloque actual */
  int siguiente_bloq;								/* Siquiente bloque del fichero */
  char lleno;									/* Booleano 1 o 0 de bloque lleno o vacio, si se escribe 1B en el array ya se cambia a lleno */
  int fin_buff_byte; 							/* indica hasta que bytes del buffer hay datos */

  char data[2048];							/* Buffer de los datos del fichero */
}  Bloque_datos;




