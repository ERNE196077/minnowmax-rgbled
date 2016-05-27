#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

//#include "headers/general.h"
//#include "headers/gpio.h"
//#include "headers/pwm.h"
#include "headers/ws281x.h"





int main (){

		led_t led_array[3] = {{255,0,0},{0,255,0},{0,0,255}};


	printf ( "%d , %d , %d  \n",led_array[0].r, led_array[0].g, led_array[0].b);
	printf ( "%d , %d , %d  \n",led_array[1].r, led_array[1].g, led_array[1].b);
	printf ( "%d , %d , %d  \n",led_array[2].r, led_array[2].g, led_array[2].b);


	ws281x_t myws281x = {
		.lednumber = 3 ,
		.ledarray = (led_t *)led_array ,
		.gpio_pin_number = 24 ,
		.dma_ch_number = 0

	};



	ws281x_init(&myws281x);


	getchar();
return 0;
}
