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

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef GENERAL_H_
#define GENERAL_H_
#endif /* GENERAL_H_ */


/*
Maps the GPIO structure in the IO memory, is needed to run UNMAP_GPIO to unmap the GPIO device.

@param		n/a

@returns 	Pointer to the mapped gpio structure.

*/
void *MAP_DEVICE(u_int32_t address){
	
	int file;
	void *temp_map;

	if (( file = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
			printf("Chingo a su madre, problema de permisos. Resultado: \n");
			return NULL;
		}

		temp_map = mmap(
				NULL,
				4096,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				file,
				address
				);

		if ( temp_map == MAP_FAILED ) {
		        perror("Memory Mapping Failed");
		        return NULL;
		   }

		return (void *)((u_int8_t *)temp_map);
}


/*
Unmaps the GPIO structure from the IO memory, it is necessary to mantain clean the system memory.

@param		volatile void *		Mapped gpio structure to be unmapped.

@returns 	n/a

*/

void UNMAP_DEVICE(volatile void* map){
	munmap((void *)map, 4096);
}


