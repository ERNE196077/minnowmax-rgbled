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
#define WORDCOUNT(led_num)		(24*led_num)

typedef struct ws281x_devices {
	volatile pwm_t *pwm_dev;
	volatile dma_channel_t *dma_dev;
	volatile gpio_t *gpio_dev;
	volatile u_int32_t *fifo_data;
	volatile u_int32_t *fifo_data2;
} ws281x_devices_t;

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

void ws281x_fifo_print(ws281x_t *ws281x) {
	u_int32_t *word = &((u_int32_t *) ws281x->devices->fifo_data)[0];
	u_int32_t *word2 = &((u_int32_t *) ws281x->devices->fifo_data2)[0];
	printf("%08x\n", word);
	printf("FIFO 1 \t\tFIFO 2\n");

	for (int i = 0; i < WORDCOUNT(ws281x->lednumber); i++)
		printf("%08x\t%08x\n", *(word + (i)), *(word2 + (i)));

}

int ws281x_fifo_init(ws281x_t *ws281x) {
	led_t *ledarray = ws281x->ledarray;
	volatile ws281x_devices_t *devices = ws281x->devices;

	//Allocate memory for the led color data
	devices->fifo_data = malloc(BYTECOUNT(ws281x->lednumber));
	devices->fifo_data2 = malloc(BYTECOUNT(ws281x->lednumber));
	if (!devices->fifo_data)
		return -1;

	u_int32_t *word = &((u_int32_t *) devices->fifo_data)[0];
	memset(word, 0, BYTECOUNT(ws281x->lednumber));

	for (int i = 0; i < ws281x->lednumber; i++) {
		int j = 24 * i;
		*(word+(j++)) =	((*ledarray).r & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) = ((*ledarray).r & 2) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).r & 4) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).r & 8) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).r & 16) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).r & 32) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).r & 64) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).r & 128) ?	WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 2) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 4) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 8) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 16) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 32) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 64) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).g & 128) ?	WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 2) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 4) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 8) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 16) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 32) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 64) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word+(j++)) =	((*ledarray).b & 128) ?	WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;

	}

	ws281x_fifo_print(ws281x);

	return 0;
}

int ws281x_init(ws281x_t *ws281x) {
	ws281x->devices = malloc(sizeof(*ws281x->devices));

	volatile ws281x_devices_t *devices;
	volatile dma_channel_t *dma_channel;
	volatile gpio_t *gpio_t;

	devices = ws281x->devices;
//volatile gpio_t *gpio = devices->gpio_dev;
//volatile pwm_t *pwm = devices->pwm_dev;
//volatile dma_t *dma = devices->dma_dev;

	if ((ws281x->gpio_pinnumber != 22) && (ws281x->gpio_pinnumber != 24))
		return -1;

	ws281x_fifo_init(ws281x);
//ws281x_pwmfifo_init (ws281x);  // Will be move after few tests.


	return 0;
}
