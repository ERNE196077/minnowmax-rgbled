
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/pci.h>
#include <math.h>
#include "headers/pwm.h"


_pwm_device_t_ pwm0 = {PWM0_BASE_ADDR,0,NULL,NULL};
_pwm_device_t_ pwm1 = {PWM1_BASE_ADDR,0,NULL,NULL};
_pwm_device_t_ *pwm_device_t[2] = {&pwm0,&pwm1};
/*
Map the PWM device into the user memory space.

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	0 when successful mapping/reading, -1 on error, -2 on device already mapped.

*/
int 	MAP_PWM				(u_int8_t pwm_n){
		if ( pwm_device_t[pwm_n] == NULL )


		if ( pwm_device_t[pwm_n]->__pwm_memory_map__ != NULL ){
			printf("PWM%d device already mapped!\n",pwm_n );
			return -2;
		}

		if (( pwm_device_t[pwm_n]->__pwm_memory_file__ = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
			printf("Cannot open //dev//mem file, check your permissions!\n");
			return -1;
		}

		pwm_device_t[pwm_n]->__pwm_memory_map__ = mmap(
				NULL,
				PWM_BLOCK_SIZE,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				pwm_device_t[pwm_n]->__pwm_memory_file__,
				pwm_device_t[pwm_n]->__pwm_base_addr_ptr__
				);

		if ( pwm_device_t[pwm_n]->__pwm_memory_map__ == MAP_FAILED ) {
		        perror("Memory Mapping Failed!");
		        return -1;
		   }
		pwm_device_t[pwm_n]->__pwm_memory_address__ = (volatile unsigned int*)pwm_device_t[pwm_n]->__pwm_memory_map__;

		close(pwm_device_t[pwm_n]->__pwm_memory_file__);
//				printf("0x0:  %04x  ---->  %04x\n",*(__pwm_memory_address+PWMCTRL0),((unsigned long)(__pwm_base_addr_ptr__)));
//			printf("0x804: %04x  ---->  %04x\n",*(__pwm_memory_address+RESETS0),((unsigned long)(__pwm_base_addr_ptr__)));
//				printf("0x808: %04x  ---->  %04x\n",*(__pwm_memory_address+RESETS0),((unsigned long)(__pwm_base_addr_ptr__)));
		
		return 0;
}


/*
Unmap the PWM device from the user memory space.

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	n/a

*/
void 	UNMAP_PWM			(u_int8_t pwm_n){
	munmap(pwm_device_t[pwm_n]->__pwm_memory_map__,PWM_BLOCK_SIZE);
	pwm_device_t[pwm_n]->__pwm_memory_map__ = NULL;
	pwm_device_t[pwm_n]->__pwm_memory_address__ = 0;
}


int INIT_PWM(u_int8_t pwm_n){

	


	return 1;
}

void	PRINT_PWM_STATUS	(u_int8_t pwm_n){
	u_int8_t 	pwm_enable_b, 
				pwm_on_time_divisor_b;
	u_int16_t 	pwm_base_unit_b;


	if ( (pwm_device_t[pwm_n]->__pwm_memory_map__ == NULL) && (pwm_device_t[pwm_n]->__pwm_memory_address__ == 0) ){
		printf("STATUS: PWM%d NOT READING!\n",pwm_n );
		return;
	}
	else
		printf("STATUS: PWM%d READING\n",pwm_n );

	pwm_enable_b = (*(pwm_device_t[pwm_n]->__pwm_memory_address__) >> 31 ) & 1 ;
	if ( pwm_enable_b == 0 )
		printf("OUTPUT: DISABLED\n");
	else
		printf("OUTPUT: ENABLED\n");

	pwm_base_unit_b = (*(pwm_device_t[pwm_n]->__pwm_memory_address__) >> 8 ) & 0xFFFF ;
	printf("FREQUENCY: %d\n", 25/(65536/pwm_base_unit_b));
	
	pwm_on_time_divisor_b = *(pwm_device_t[pwm_n]->__pwm_memory_address__) & 0xFF ;
	printf("ON-TIME DIVISOR: %04x",pwm_on_time_divisor_b);
}

