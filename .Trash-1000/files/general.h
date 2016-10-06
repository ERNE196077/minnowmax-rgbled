#ifndef GENERAL_H_
#define GENERAL_H_

/*
 * general.h
 *
 * General functions, my assumption is that all the blocks of the IO space address
 * in the Minnowboard Max are of size 4096. This size will be used to create the
 * below functions.
 *
 *  Created on: Jul 30, 2015
 *      Author: minnow
 */


#define BLOCK_SIZE_T	4096
#define DEV_WS281X  0
#define DEV_APA102  1


/*
 Maps the size of bytes of a structure into IO memory.

 @param		address		Address of the IO map to be mapped
 @param		size 		Amount of bytes to be reserved

 @returns 	Pointer to the mapped gpio structure.


void *MAP_DEVICE(u_int32_t address, u_int32_t size) {

	int file;
	void *temp_map;

	if ((file = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("Chingo a su madre, problema de permisos. Resultado: \n");
		return NULL;
	}

	temp_map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file,
			address);
	if (temp_map == MAP_FAILED) {
		perror("Memory Mapping Failed");
		return NULL;
	}
	close(file);

	return (void *) ((u_int8_t *) temp_map);
}


 Unmaps the GPIO structure from the IO memory, it is necessary to mantain clean the system memory.

 @param		volatile void *		Mapped gpio structure to be unmapped.
 @param		size				Size of memory to be released

 @returns 	n/a



void UNMAP_DEVICE(volatile void* map, u_int32_t size) {
	int r = munmap((void *) map, size);
	if (r != 0)
		perror("Unmapping memory problem");
}

*/

#endif /* GENERAL_H_ */