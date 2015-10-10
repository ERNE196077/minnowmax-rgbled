#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include "headers/pwm.h"


int main (){

	printf("Hello World! Biatch\n");



INIT_GPIO(22);
//SET_GPIO_FUNC(22,1);
GET_GPIO_STATUS(22);
CLOSE_GPIO(22);

MAP_PWM(0);
SET_PWM_DUTY(0,13);
INIT_PWM(0);
/*
while ( 1 ){
SET_PWM_DIV(0,0);
nanosleep((const struct timespec[]){{0, 30710L}}, NULL);
SET_PWM_DIV(0,255);
nanosleep((const struct timespec[]){{0, 50710L}}, NULL);

}
*/
SET_PWM_DIV(0,7);

/*
int i = 0 ;
while (1){
while (i<3){
	SET_PWM_DIV(0,i);
	usleep(100000);
i++;
}
while (i>=0){
	SET_PWM_DIV(0,i);
	usleep(100000);
i--;
}
}
*/
PRINT_PWM_STATUS(0);
UNMAP_PWM(0);
/*
INIT_GPIO(22);
GET_GPIO_STATUS(22);
CLOSE_GPIO(22);

INIT_PWM();
*/
return 0;
}
