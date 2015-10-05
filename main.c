#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "headers/pwm.h"


int main (){

	printf("Hello World! Biatc\n");



INIT_GPIO(22);
SET_GPIO_FUNC(22,1);
GET_GPIO_STATUS(22);
CLOSE_GPIO(22);

/*
INIT_GPIO(22);
GET_GPIO_STATUS(22);
CLOSE_GPIO(22);

INIT_PWM();
*/
return 0;
}
