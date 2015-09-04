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
GET_GPIO_VALUE(25);
CLOSE_GPIO(25);

return 0;

}
