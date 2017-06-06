#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include "headers/rgbled.h"


int main (){

	rgbled_conf_t ledsconfig = RGBLED_CONF_TYPEAPA102 | RGBLED_CONF_DMACH(1) | RGBLED_CONF_LEDNUM(15) | RGBLED_CONF_ANIMATION(3) ;

	rgbled_init(&ledsconfig);

return 0;
}
