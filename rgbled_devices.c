
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>

#include "headers/rgbled_devices.h"



void rgbled_test(__u32 *data){

	led_ws281x_t *ws281x_data = (led_ws281x_t *) data ; 

	int i ;
	for ( i = 0 ; i < 15 ; i++ ){
		(ws281x_data + i)->r = RGBLEDWS281X_RGBCONV(0);
		(ws281x_data + i)->g = RGBLEDWS281X_RGBCONV(255);
		(ws281x_data + i)->b = RGBLEDWS281X_RGBCONV(255);
		printk(KERN_INFO"LED %d RED %08x \n",i,(ws281x_data + i)->r);
		printk(KERN_INFO"LED %d GREEN %08x \n",i,(ws281x_data + i)->g);
		printk(KERN_INFO"LED %d BLUE %08x \n\n",i,(ws281x_data + i)->b);
	}


}