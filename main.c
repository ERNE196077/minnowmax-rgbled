#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include "headers/rgbled.h"


int main (){

	rgbled_conf_t ledsconfig = RGBLED_CONF_WS281X | RGBLED_CONF_DMACH(0) | RGBLED_CONF_LEDNUM(15) ;

	rgbled_init(&ledsconfig);

	getchar();
return 0;
}
