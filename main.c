#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "headers/rgbled.h"


int main (){
	/*  Run on Minnowboard */
	rgbled_conf_t ledsconfig = RGBLED_CONF_TYPEAPA102 | 
							RGBLED_CONF_DMACH(0) | 
							RGBLED_CONF_LEDNUM(80);

	if (rgbled_init(&ledsconfig))
		return 1;

	rgbled_test();
	sleep(5);
	
	rgbled_x11(20,10,20,10,35);

return 0;
}
