
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>

#include "headers/rgbled_devices.h"

void rgbled_setcolor(__u32 *data, led_t ledcolor){
	__u16 lednumber = RGBLED_CONF_GET_LEDNUM(devices.rgbled_config);
	__u32 ledtype = RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config);
	__u16 i;
	led_ws281x_t *ws281x_data;
	led_apa102_t *apa102_data;
	switch (ledtype){

		case ledtype_apa102:
			apa102_data = (led_apa102_t *) data ;
			/* Start frame */
			*((__u32 *)apa102_data) = 0x0;
			/* Led frames*/
			for ( i = 1 ; i <= lednumber ; i++ ){
				(apa102_data + i)->global = 0xFF;
				(apa102_data + i)->r = ledcolor.r;
				(apa102_data + i)->g = ledcolor.g;
				(apa102_data + i)->b = ledcolor.b;
			}
			/* End Frame */
			*((__u32 *)(apa102_data + lednumber + 1)) = 0xFFFFFFFF;

			/* Frames needed to complete the clocks to display the entire led strip */
			for (i = (lednumber + 2) ; i < ((lednumber + 2) + devices.apa102_endframes) ; i++)
			*((__u32 *)(apa102_data + i)) = 0x0;
			
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
		case ledtype_apa102:
			apa102_data = (led_apa102_t *) data ;
			/* Start frame */
			*((__u32 *)apa102_data) = 0x00000000;
			/* Led frames*/
			for ( i = 1 ; i <= lednumber ; i++ ){
				(apa102_data + i)->global = 0xFF;
				(apa102_data + i)->r = (ledmatrix + (i - 1) )->r;
				(apa102_data + i)->g = (ledmatrix + (i - 1) )->g;
				(apa102_data + i)->b = (ledmatrix + (i - 1) )->b;
			}
			/* End Frame */
			*((__u32 *)(apa102_data + lednumber + 1)) = 0xFFFFFFFF;
			
			/* Frames needed to complete the clocks to display the entire led strip */
			for (i = (lednumber + 2) ; i < ((lednumber + 2) + devices.apa102_endframes) ; i++)
				*((__u32 *)(apa102_data + i)) = 0x0;
			
		break;

		case ledtype_ws281x:
			ws281x_data = (led_ws281x_t *) data ;
			for ( i = 0 ; i < lednumber ; i++ ){
				(ws281x_data + i)->r = RGBLEDWS281X_RGBCONV((ledmatrix + i)->r);
				(ws281x_data + i)->g = RGBLEDWS281X_RGBCONV((ledmatrix + i)->g);
				(ws281x_data + i)->b = RGBLEDWS281X_RGBCONV((ledmatrix + i)->b);
			}
		break;
	}
}
