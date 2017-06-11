
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>

#include "headers/rgbled_devices.h"

	
int rx=0, gx=0, bx=1;
int r=255, g=128, b=0;
void rgbled_setcolor(__u32 *data, led_t ledcolor){
	__u16 lednumber = RGBLED_CONF_GET_LEDNUM(devices.rgbled_config);
	__u32 ledtype = RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config);
	__u16 i;
	led_ws281x_t *ws281x_data;
	led_apa102_t *apa102_data;
	printk("%d ledtype\n",ledtype);
	switch (ledtype){

		case ledtype_apa102:
			apa102_data = (led_apa102_t *) data ;
			/* Start frame */
			*((__u32 *)apa102_data) = 0x00000000;
			/* Led frames*/
			for ( i = 1 ; i <= lednumber ; i++ ){
				(apa102_data + i)->global = 0xFF;
				(apa102_data + i)->r = ledcolor.r;
				(apa102_data + i)->g = ledcolor.g;
				(apa102_data + i)->b = ledcolor.b;
			}
			/* End Frame */
			*((__u32 *)(apa102_data + lednumber + 1)) = 0xFFFFFFFF;

			for (i = 0 ; i <= (lednumber + 1) ; i++ )
				printk("%08x\n",*((__u32 *)(apa102_data + i)));
			break;

		case ledtype_ws281x:
			ws281x_data = (led_ws281x_t *) data ;
			for ( i = 0 ; i < lednumber ; i++ ){
				(ws281x_data + i)->r = RGBLEDWS281X_RGBCONV(ledcolor.r);
				(ws281x_data + i)->g = RGBLEDWS281X_RGBCONV(ledcolor.g);
				(ws281x_data + i)->b = RGBLEDWS281X_RGBCONV(ledcolor.b);
			}
		break;

	}
}


void rgbled_usermatrix(__u32 *data, led_t *ledmatrix){
	__u16 lednumber = RGBLED_CONF_GET_LEDNUM(devices.rgbled_config);
	__u32 ledtype = RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config);
	__u16 i;
	led_ws281x_t *ws281x_data;
	led_apa102_t *apa102_data;

	switch (ledtype){

		case RGBLED_CONF_TYPEAPA102:
		break;

		case RGBLED_CONF_TYPEWS281X:
			ws281x_data = (led_ws281x_t *) data ;
			for ( i = 0 ; i < lednumber ; i++ ){
				(ws281x_data + i)->r = RGBLEDWS281X_RGBCONV((ledmatrix + i)->r);
				(ws281x_data + i)->g = RGBLEDWS281X_RGBCONV((ledmatrix + i)->g);
				(ws281x_data + i)->b = RGBLEDWS281X_RGBCONV((ledmatrix + i)->b);

				
			}
			printk(KERN_INFO"User defined %08x\n",*((__u32 *)ws281x_data));
		break;

	}
}
