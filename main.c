#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include "headers/gpio.h"


int main (){

	printf("Hello World! Biatch\n");

	volatile gpio_t *gpio_map = MAP_DEVICE(0XFED0C000);
	

	printf("%08x\n", gpio_map->__gpio_13_cfg__);
	printf("%08x\n\n", gpio_map->__gpio_13_val__);
	GPIO_CFG_FUNCTION(gpio_map->__gpio_13_cfg__,0);
	gpio_map->__gpio_13_cfg__ |= (1 <<	10);
	GPIO_VAL_SET(gpio_map->__gpio_13_val__);
	printf("%08x\n", gpio_map->__gpio_13_cfg__);
	printf("%08x\n", gpio_map->__gpio_13_val__);
	UNMAP_GPIO(gpio_map);

return 0;
}
