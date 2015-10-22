/*
 * gpio.c
 *
 * GPIO Functionality.
 * When a GPIO device is read (INIT_GPIO), it's mapping is stored in the respective number of the "pin_gpio_t" table. 
 * "gpio_pins_t" stores only the main information of the GPIO's (Functions, address, offset).
 *
 *
 *  Created on: Sep 3, 2015
 *      Author: minnow
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "headers/gpio.h"

/*****			GPIO VARIABLES			*****/




/*
Maps the GPIO structure in the IO memory, is needed to run UNMAP_GPIO to unmap the GPIO device.

@param		n/a

@returns 	Pointer to the mapped gpio structure.

*/
void *MAP_GPIO(){
	
	int file;
	u_int32_t size = sizeof(gpio_t);
	void *temp_map;

	if (( file = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
			printf("Chingo a su madre, problema de permisos. Resultado: \n");
			return NULL;
		}

		temp_map = mmap(
				NULL,
				size,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				file,
				GPIO_BASE_SCORE
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

void UNMAP_GPIO(volatile void* map){
	munmap((void *)map, 4096);
}

