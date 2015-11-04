
/*
 * pwm.c
 *
 * PWM Functionality
 * Below are the steps to program the PWM output from scratch:
 * 	1. Start the user space register mapping
 * 	2. Program the counter value PWMCTRL.PWM_BASE_UNIT and PWMCTRL.PWM_ON_TIME_DIVISOR
 * 	3. Set PWMCTRL.PWM_SW_UPDATE bit
 *	4. Enable PWM output by setting the PWMCTRL.PWM_ENABLE bit
 * For update on the fly, follow sequence:
 * 	1. Program the counter value PWMCTRL.PWM_BASE_UNIT and/or PWMCTRL.PWM_ON_TIME_DIVISOR
 *	2. Set PWMCTRL.PWM_SW_UPDATE bit
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


/*
Print the status of the PWM interface.

@param	pwm_reg	PWM device register.

@returns	n/a

*/
void	PRINT_PWM_STATUS	(u_int32_t pwm_reg){
	u_int8_t 	pwm_enable_b, 
				pwm_on_time_divisor_b;
	u_int16_t 	pwm_base_unit_b;

	pwm_enable_b = (pwm_reg >> 31 ) & 1 ;
	if ( pwm_enable_b == 0 )
		printf("Output:\t\t\tDisabled\n");
	else
		printf("Output:\t\t\tEnabled\n");

	pwm_base_unit_b = (pwm_reg >> 8 ) & 0xFFFF ;
	float pwm_freq = 25/(65536/(float)(256*pwm_base_unit_b));
	char* freq_unit = pwm_freq < 0 ? "Mhz" : "Khz";
	pwm_freq *= pwm_freq > 0 ? 1000 : 1 ;
	printf("Frequency:\t\t%5.2f %s -> %d\n", pwm_freq ,freq_unit, pwm_base_unit_b);
	
	pwm_on_time_divisor_b = pwm_reg & 0xFF ;
	printf("On-Time Divisor:\t%04x\n",pwm_on_time_divisor_b);

	printf("Register Value:\t\t0x%08x\n",pwm_reg);

}

