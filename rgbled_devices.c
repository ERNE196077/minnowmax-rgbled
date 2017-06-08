
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>

#include "headers/rgbled_devices.h"

	
int rx=0, gx=0, bx=1;
int r=255, g=128, b=0;
void rgbled_test(__u32 *data, __u32 test_var ){
	__u32 i;

	led_ws281x_t *ws281x_data = (led_ws281x_t *) data ; 

	if(r == 0) rx = 0; else if(r==255) rx = 1;
	if(g == 0) gx = 0; else if(g==255) gx = 1;
	if(b == 0) bx = 0; else if(b==255) bx = 1;

	//b = (bx) ? b-1 : b+1;
	//r = (rx) ? r-1 : r+1; 
	//g = (gx) ? g-1 : g+1;


	for ( i = 0 ; i < test_var ; i++ ){
			if (rx)
				r++;
			else 
				rx--;
			if(gx)
				g++;
			else 
				g--;
			if (bx)
				b++;
			else
				b--;
			(ws281x_data + i)->r = RGBLEDWS281X_RGBCONV(r);
			(ws281x_data + i)->g = RGBLEDWS281X_RGBCONV(g);
			(ws281x_data + i)->b = RGBLEDWS281X_RGBCONV(b);
		
	}  
	/*
	(ws281x_data + 451)->r = 0x0;
	(ws281x_data + 451)->g = 0x0;
	(ws281x_data + 451)->b = 0x0;

	(ws281x_data + 452)->r = 0x0;
	(ws281x_data + 452)->g = 0x0;
	(ws281x_data + 452)->b = 0x0;
	*/	
}