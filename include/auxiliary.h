/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	01/03/2017
 */


/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int obtener_blq_libre();
int contar_ficheros(); //comprueba que no se puedan crear mas ficheros
int check_bmap_free(); //devuelve posicion libre
int actualizar_bmap();
int separar_path(char * path); //devuelve el nivel del elemento
char * nombre_directorio(char * path); //devuelve el nivel del elemento

