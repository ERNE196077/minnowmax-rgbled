#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include "headers/rgbled.h"


int main (){

	rgbled_conf_t ledsconfig = RGBLED_CONF_TYPEWS281X | RGBLED_CONF_DMACH(0) | RGBLED_CONF_LEDNUM(287) | RGBLED_CONF_ANIMATION(3) ;

	rgbled_init(&ledsconfig);

return 0;
}
