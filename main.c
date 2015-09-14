#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "headers/gpio.h"


int main (){

	printf("Hello World! Biatch\n");

INIT_GPIO(25);

SET_GPIO_DIR_INP(25);
GET_GPIO_VALUE(25);

SET_GPIO_DIR_OUT(25);
GET_GPIO_VALUE(25);


int i=0;

while(i<10){
	SET_GPIO_VALUE(25);
	sleep(1);

	CLEAR_GPIO_VALUE(25);
	sleep(1);
	i++;
}

CLOSE_GPIO(25);

return 0;

}
