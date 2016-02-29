
/*
 * ws281x.c
 *
 * WS281x Functionality
 * The basic functionality is to set the clock of the PWM as close as possible 
 * to the frequency specified in the WS281x documentation.
 * A FIFO is created to contain the registers of the PWM_ON_TIME_DIVISOR which
 * will be varying to draw the logical bits in the PWM signal.
 * 
 * My assumption is that the PWM Controller contains a FIFO which will be fill
 * with the data from the DMA transfer.
 *
 *  Created on: Oct 28, 2015
 *      Author: minnow
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/pci.h>
#include "headers/ws281x.h"
#include "headers/pwm.h"
#include "headers/dma.h"
#include "headers/gpio.h"


#define BYTECOUNT(led_num) 		(24*led_num*4)   	//24 registers per led... Each register of 4 bytes (u_int32_t)


typedef struct ws281x_devices{
volatile pwm_t 		*pwm_dev;
volatile dma_t 		*dma_dev;
volatile gpio_t		*gpio_dev;
volatile u_int32_t 	*fifo_data;

}ws281x_devices_t;

/*

int 		ws281x_init					(ws281x_t *ws281x);
int 		ws281x_deinit				(ws281x_t *ws281x);
int 		ws281x_devices_map			(ws281x_devices_t *devices);
void 		ws281x_devices_unmap 		(ws281x_devices_t *devices);
void 		ws281x_pwmfifo_deinit		(ws281x_t *ws281x);
void		ws281x_pwmfifo_set			(ws281x_t *ws281x);
void		ws281x_dma_init				(ws281x_devices_t *devices);
void		ws281x_dma_deint			(ws281x_devices_t *devices);
*/

int 		ws281x_pwmfifo_init		(ws281x_t *ws281x){
led_t *ledarray = ws281x->ledarray;
volatile u_int32_t *fifo_data = ws281x->devices->fifo_data;

	for( int i = 0 ; i < (sizeof(ledarray)/sizeof(ledarray[0])) ; i++ ){
		int j = 24 * i ;
		fifo_data[j++] = ( ledarray[i].r & 1 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 2 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 4 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 8 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 16 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 32 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 64 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].r & 128 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 1 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 2 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 4 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 8 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 16 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 32 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 64 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].g & 128 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 1 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 2 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 4 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 8 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 16 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 32 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 64 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		fifo_data[j++] = ( ledarray[i].b & 128 ) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		
	}

	return 0;
}

int ws281x_init (ws281x_t *ws281x){

ws281x_devices_t *devices = ws281x->devices;
volatile gpio_t *gpio = devices->gpio_dev;
volatile pwm_t *pwm = devices->pwm_dev;
volatile dma_t *dma = devices->dma_dev;
volatile u_int32_t *fifo_data = devices->fifo_data;
u_int8_t gpio_pinnumber = ws281x->gpio_pinnumber;
	
	if( (gpio_pinnumber != 22) && (gpio_pinnumber != 24) )
		return -1;

	fifo_data = (malloc(BYTECOUNT(ws281x->lednumber));

	if( !fifo_data)
		return -1;

memset(fifo_data, 0, BYTECOUNT(ws281x->lednumber));
ws281x_pwmfifo_init (ws281x);  // Will be move after few tests.

for ( int i = 0 ; i < 24 ; i++)
printf("%d\n", devices->fifo_data[i] );

return 0 ;
}
