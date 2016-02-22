
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
#include "headers/dma.h"
/*







int 		ws281x_init					(ws281x_t *ws281x);
int 		ws281x_deinit				(ws281x_t *ws281x);
int 		ws281x_devices_map			(ws281x_devices_t *devices);
void 		ws281x_devices_unmap 		(ws281x_devices_t *devices);
pwm_fifo_t 	*ws281x_pwmfifo_init		(ws281x_t *ws281x);
void 		ws281x_pwmfifo_deinit		(ws281x_t *ws281x);
void		ws281x_pwmfifo_set			(ws281x_t *ws281x);
void		ws281x_dma_init				(ws281x_devices_t *devices);
void		ws281x_dma_deint			(ws281x_devices_t *devices);
*/
