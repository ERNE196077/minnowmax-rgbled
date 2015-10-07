
/*
 * pwm.c
 *
 * PWM Functionality
 * Below are the steps to program the PWM output from scratch:
 * 	1. Start the user space register mapping (MAP_PWM)
 * 	2. Program the counter value PWMCTRL.PWM_BASE_UNIT (SET_PWM_DUTY) and PWMCTRL.PWM_ON_TIME_DIVISOR (SET_PWM_DIV)
 * 	3. Enable PWM output by setting the PWMCTRL.PWM_ENABLE bit (INIT_PWM)
 *
 * For update on the fly, follow sequence:
 * 	1. Program the counter value PWMCTRL.PWM_BASE_UNIT (SET_PWM_DUTY) and/or PWMCTRL.PWM_ON_TIME_DIVISOR (SET_PWM_DIV)
 *
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
#include <linux/pci.h>
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


/*
Init the PWM output

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	n/a

*/
void 	INIT_PWM			(u_int8_t pwm_n){
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) |= ( 1 << 31 );
}


/*
Stop the PWM output

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	n/a

*/
void 	STOP_PWM			(u_int8_t pwm_n){
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) &= ~( 1 << 31 );
}


/*
Set and update the PWM duty cycle

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	0 if successful, -1 if the value is out of range.

*/
int 	SET_PWM_DUTY		(u_int8_t pwm_n, u_int16_t duty){
	if ( duty < 65536 ){
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) &=  ~( 0xFFFF << 8 ) ;
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) |=  ( duty << 8 ) ;
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) |= ( 1 << 30 );
	return 0;
	}
	printf("PWM duty value out of range\n");
	return -1;
}


/*
Set and update the PWM On-Time divisor.

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	0 if successful, -1 if the value is out of range

*/
int 	SET_PWM_DIV			(u_int8_t pwm_n, u_int8_t div){
	if ( div < 256 ){ 
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) &=  ~(0xFF) ;
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) |=  div ;
	*(pwm_device_t[pwm_n]->__pwm_memory_address__) |= ( 1 << 30 );
	printf("%04x\n",*(pwm_device_t[pwm_n]->__pwm_memory_address__) &  (0xFF) );
	return 0;
	}
	printf("PWM divisor value out of range\n");
	return -1;
}


/*
Print the status of the PWM interface.

@param	pwm_n	PWM device number, available: 0 or 1.

@returns	n/a

*/
void	PRINT_PWM_STATUS	(u_int8_t pwm_n){
	u_int8_t 	pwm_enable_b, 
				pwm_on_time_divisor_b;
	u_int16_t 	pwm_base_unit_b;


	if ( (pwm_device_t[pwm_n]->__pwm_memory_map__ == NULL) && (pwm_device_t[pwm_n]->__pwm_memory_address__ == 0) ){
		printf("Status:\tPWM%d not reading!\n",pwm_n );
		return;
	}
	else
		printf("Status:\tPWM%d reading...\n",pwm_n );

	pwm_enable_b = (*(pwm_device_t[pwm_n]->__pwm_memory_address__) >> 31 ) & 1 ;
	if ( pwm_enable_b == 0 )
		printf("Output:\tDisabled\n");
	else
		printf("Output:\tEnabled\n");

	pwm_base_unit_b = (*(pwm_device_t[pwm_n]->__pwm_memory_address__) >> 8 ) & 0xFFFF ;
	float pwm_freq = 25/(65536/(float)pwm_base_unit_b);
	char* freq_unit = pwm_freq < 0 ? "Mhz" : "Khz";
	pwm_freq *= pwm_freq > 0 ? 1000 : 1 ;
	printf("Frequency:\t%5.2f %s -> %d\n", pwm_freq ,freq_unit, pwm_base_unit_b);
	
	pwm_on_time_divisor_b = *(pwm_device_t[pwm_n]->__pwm_memory_address__) & 0xFF ;
	printf("On-Time Divisor:\t%04x\n",pwm_on_time_divisor_b);

	printf("Register Value:\t0x%04x\n",*(pwm_device_t[pwm_n]->__pwm_memory_address__));

}

