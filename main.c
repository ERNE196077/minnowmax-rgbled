#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "headers/rgbled.h"


int main (){
	/*  Run on Minnowboard */
	rgbled_conf_t ledsconfig = RGBLED_CONF_TYPEAPA102 | 
							RGBLED_CONF_DMACH(0) | 
							RGBLED_CONF_LEDNUM(239);

	if (rgbled_init(&ledsconfig))
		return 1;

	rgbled_test();
	sleep(2);
	
	rgbled_x11(77,42,78,42,35);

return 0;
}
