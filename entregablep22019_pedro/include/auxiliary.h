/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	01/03/2017
 */
/*
 * @brief 	Gets the position of a iNode given its file descriptor.
 * @return 	position of the iNode if success, -1 otherwise.
 */
int getPos(int fd);