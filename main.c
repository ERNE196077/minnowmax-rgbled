#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include "headers/rgbled.h"


int main (){
	/*  Run on Minnowboard 
	rgbled_conf_t ledsconfig = RGBLED_CONF_TYPEAPA102 | 
							RGBLED_CONF_DMACH(0) | 
							RGBLED_CONF_LEDNUM(144);

	if (rgbled_init(&ledsconfig))
		return 1;

	rgbled_test();
	*/
	
	rgbled_x11();

return 0;
}
