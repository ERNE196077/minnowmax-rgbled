#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include "headers/general.h"
#include "headers/gpio.h"
#include "headers/pwm.h"
#include "headers/ws281x.h"





int main (){


	volatile u_int32_t *gpio_base = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS, GPIO_SCORE_BASE_SIZE);
	volatile pwm_t *pwm_base = MAP_DEVICE(PWM0_BASE_ADDR, sizeof(pwm_t));

	volatile gpio_t *gpio22 = GPIO_MAP(22,gpio_base)

		led_t led_array[3] = {{255,0,0},{0,255,0},{0,0,255}};


	printf ( "%d , %d , %d  \n",led_array[0].r, led_array[0].g, led_array[0].b);
	printf ( "%d , %d , %d  \n",led_array[1].r, led_array[1].g, led_array[1].b);
	printf ( "%d , %d , %d  \n",led_array[2].r, led_array[2].g, led_array[2].b);


	ws281x_t myws281x = {
		.lednumber = 3 ,
		.ledarray = (led_t *)led_array ,
		.gpio_pinnumber = 22 ,
		.dma_channel = 2

	};



	ws281x_init(&myws281x);

		//printf("%08x\n\n", gpio_map->__gpio_22_val__);
//	GPIO_CFG_FUNCTION(gpio22->__cfg__,1);
//	GPIO_VAL_OUTPUT(gpio22->__val__);
//	GPIO_VAL_SET(gpio22->__val__);
/*	for(int i = 0 ; i < 255 ; i++){
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_BASE_UNIT(3000) | PWM_ON_TIME_DIVISOR(i);
		usleep(70000);
	}
	for(int i = 255 ; i > 0  ; i--){
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_BASE_UNIT(3000) | PWM_ON_TIME_DIVISOR(i);
		usleep(70000);
	}

*/
			printf("%08x\n", gpio22->__val__);
			printf("%08x\n", gpio22->__cfg__);
			printf("%08x\n", pwm_base->__pwm_ctrl__);
				//PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,20);
	//PWM_CTRL_BASE_UNIT(pwm_map->__pwm_ctrl__,8);

	//PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
	//PWM_CTRL_ENABLE(pwm_map->__pwm_ctrl__);

			UNMAP_DEVICE(gpio_base,GPIO_SCORE_BASE_SIZE);
			UNMAP_DEVICE(gpio_base,sizeof(pwm_t));
return 0;
}
