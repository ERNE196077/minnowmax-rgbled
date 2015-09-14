#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "headers/gpio.h"


int main (){

	printf("Hello World! Biatch\n");

INIT_GPIO(24);
SET_GPIO_VALUE(24);
GET_GPIO_STATUS(24);
CLOSE_GPIO(24);

return 0;
}
