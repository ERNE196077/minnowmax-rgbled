
#include "headers/gpio.h"

int main (){

	printf("Hello World! Biatch\n");

	if (( GPIO_memory_file = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
		printf("Chingo a su madre, problema de permisos. Resultado: %d\n",GPIO_memory_file);
		return -1;
	}

	GPIO_memory_map = mmap(
			NULL,
			BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			GPIO_memory_file,
			GPIO_address_pointer
			);

	if (GPIO_memory_map == MAP_FAILED) {
	        perror("Memory Mapping Failed");
	        return -1;
	   }
	GPIO_memory_address = (volatile unsigned int*)GPIO_memory_map;

	printf("Value in %04x :\n", (unsigned int)GPIO_address_pointer);

	printf("HEX: \n");
	for( int i=0 ; i<200 ; i+=4)
	{
		printf("Offset %02xh : ",i*4);
		printf("%04x \n",*(GPIO_memory_address+GPIO_OFFSET+i));
		printf("Offset %02xh : ",(i+2)*4);
		printf("%04x",*(GPIO_memory_address+GPIO_OFFSET+(i+1)));
		printf("%04x\n",*(GPIO_memory_address+GPIO_OFFSET+(i+2)));
	}

	printf("\n");

	munmap(GPIO_memory_map,BLOCK_SIZE);
	close(GPIO_memory_file);
	return 0;
}
