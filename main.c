#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#include "headers/pwm.h"


int main (){

	printf("Hello World! Biatch\n");



INIT_GPIO(22);
//SET_GPIO_FUNC(22,1);
GET_GPIO_STATUS(22);
CLOSE_GPIO(22);

MAP_PWM(1);
INIT_PWM(1);
SET_PWM_DUTY(1,5);

int i = 0 ;
while (i<5){
	SET_PWM_DIV(1,i);
	usleep(10000);
i++;
}


PRINT_PWM_STATUS(1);
UNMAP_PWM(1);
/*
INIT_GPIO(22);
GET_GPIO_STATUS(22);
CLOSE_GPIO(22);

INIT_PWM();
*/
return 0;
}
