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
int check_blq_libre(void);
int disk_sync(void);
int ialloc_custom(void);
int alloc_custom(void);
int alloc_databloq_custom(int id_bloque_Datos);
int check_free_inode(int id_bloque_Datos);
int check_free_databloq(int id_bloque_Datos);


/* TODO: Comprobar que se han incluido todas*/

