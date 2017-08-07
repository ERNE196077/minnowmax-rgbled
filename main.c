#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "headers/rgbled.h"
	
led_t user;

int main (int argc, char* args[]){

	int i = 0; 
	for (i = 0 ; i < argc ; i++){
		switch( args[i] ){
			case "-c":
				rgbled_setconfig((uint8_t) args[++i] | (uint8_t) args[++i] | (uint16_t) args[++i]);
			break;
			case "-x":
				rgbled_x11((int)args[++i], (int) args[++i], (int) args[++i], (int) args[++i], (int) args[++i], (int) args[++i] );
			break;
			case "-u":
				user = {(uint8_t)args[++i], (uint8_t) args[++i], (uint8_t) args[++i]}
				rgbled_setcolor(&user)
			break;
			case "-f":
				rgbled_function((led_function_t) args[++i])
			break;

			case "-h":
			default:
				printf("Usage:\n
						rgbled [OPTION] {Arguments}\n
						\n
						Options:\n
						-c {ledtype dmach lednumber}\t\tWS2812B = 0;APA102 = 1; DMA channel 0 - 7; LED number 1 - 4096\n
						-x {top right bottom left wborder hborder}\t\tTop, rigth, bottom, left - number of LEDs on each segment, wborder/hborder pixel border on each segment\n
						-u {red green blue}\t\tSetup LED strip with the defined red, green, blue color, each color from 0 to 255\n
						-f {function}\t\tSet the specified function/color preset\n");
		}
	}

return 0;
}
