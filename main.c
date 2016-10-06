#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include "headers/rgbled.h"


int main (){

	rgbled_conf_t ledsconfig = RGBLED_CONF_TYPEAPA102 | RGBLED_CONF_DMACH(7) | RGBLED_CONF_LEDNUM(15) ;

	rgbled_init(&ledsconfig);

	getchar();
return 0;
}
