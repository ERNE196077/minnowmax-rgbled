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


	ws281x_t myws281x = {
		.lednumber = 3 ,
		.ledarray = NULL ,
		.gpio_pin_number = 24 ,
		.dma_ch_number = 1

	};



	ws281x_init(&myws281x);


	getchar();
return 0;
}
