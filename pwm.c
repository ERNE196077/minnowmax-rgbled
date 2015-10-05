
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/pci.h>
#include "headers/pwm.h"

		unsigned long 			__pwm_base_addr_ptr__;
		u_int32_t 				__pwm_offset__;
		int 					__pwm_memory_file__;
		void 					*__pwm_memory_map_;
		volatile unsigned int 	*__pwm_memory_address;




/*****************************

param:

@: 

*****************************/
int 	INIT_PWM0			(){

}


int 	STOP_PWM0			();
int 	SET_PWM0_DUTY		(u_int16_t __duty__);
int 	SET_PWM0_DIV		(u_int8_t __div__);
void 	PRINT_PWM0_REG		();

int 	INIT_PWM1			();
int 	STOP_PWM1			();
int 	SET_PWM1_DUTY		(u_int16_t __duty__);
int 	SET_PWM1_DIV		(u_int8_t __div__);
void 	PRINT_PWM1_REG		();


int INIT_PWM(){

	__pwm_offset__ = 0x0;
	__pwm_base_addr_ptr__ = 0x90825000;

		if (( __pwm_memory_file__ = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
			printf("Chingo a su madre, problema de permisos. Resultado: %d\n",__pwm_memory_file__);
			return -1;
		}

		__pwm_memory_map_ = mmap(
				NULL,
				PWM_BLOCK_SIZE,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				__pwm_memory_file__,
				__pwm_base_addr_ptr__
				);

		if ( __pwm_memory_map_ == MAP_FAILED ) {
		        perror("Memory Mapping Failed");
		        return -1;
		   }
		__pwm_memory_address = (volatile unsigned int*)__pwm_memory_map_;
		close(__pwm_memory_file__);
				printf("0x0:  %04x  ---->  %04x\n",*(__pwm_memory_address+PWMCTRL0),((unsigned long)(__pwm_base_addr_ptr__)));
				printf("0x804: %04x  ---->  %04x\n",*(__pwm_memory_address+RESETS0),((unsigned long)(__pwm_base_addr_ptr__)));
				printf("0x808: %04x  ---->  %04x\n",*(__pwm_memory_address+RESETS0),((unsigned long)(__pwm_base_addr_ptr__)));
		munmap(__pwm_memory_map_,PWM_BLOCK_SIZE);


	return 0;
}

int PRINT_PWM(){


	__pwm_offset__ = 0x0;
	__pwm_base_addr_ptr__ = 0xFED08000;


		if (( __pwm_memory_file__ = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
			printf("Chingo a su madre, problema de permisos. Resultado: %d\n",__pwm_memory_file__);
			return -1;
		}

		__pwm_memory_map_ = mmap(
				NULL,
				PWM_BLOCK_SIZE,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				__pwm_memory_file__,
				__pwm_base_addr_ptr__
				);

		if ( __pwm_memory_map_ == MAP_FAILED ) {
		        perror("Memory Mapping Failed");
		        return -1;
		   }
		__pwm_memory_address = (volatile unsigned int*)__pwm_memory_map_;
		close(__pwm_memory_file__);
		for (u_int32_t i = 0x0 ; i  < 0x500 ; i+=0x1   ){

			printf("%04x  ---->  %04x\n",*(__pwm_memory_address+i),((unsigned long)(__pwm_base_addr_ptr__))+i);
		}
		munmap(__pwm_memory_map_,PWM_BLOCK_SIZE);



	return 0;
}

