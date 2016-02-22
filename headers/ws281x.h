#ifndef WS281X_H_
#define WS281X_H_
#endif /* GPIO_H_ */

/*
 * ws281x.h
 *
 * Adding needed structures and definitions to handle the ws281x LED's.
 *
 *  Created on: Jul 30, 2015
 *      Author: minnow
 */

#include "headers/dma.h"
#include "headers/gpio.h"
#include "headers/pwm.h"


/*****			WS281X DEFAULT VALUES			*****/

#define WS281X_DEFAULT_GPIO_PIN			24		// PWM Capable pins: 22 & 24
#define WS281X_DEFAULT_LED_NUMBER		1		
#define WS281X_DEFAULT_DMA_CHANNEL		0		
#define	WS281X_PWM_BASE_UNIT			0x848
#define WS281X_PWM_BIT_HIGH				0xA0
#define WS281X_PWM_BIT_LOW				0x50


 /*****			WS281X STRUCTURES			*****/

typedef struct ws281x_devices_t;

typedef struct
{
	u_int32_t			__lednumber__:
	u_int32_t			__gpio_pinnumber__;
	u_int32_t			__dma_channel__;
	pwm_fifo_t			__pwm_fifo__;
	ws281x_devices_t 	__devices__;
	
}__attribute__ ((packed)) ws281x_t;


/*****			WS281X FUNCTIONS			*****/

int 		ws281x_init					(ws281x_t *ws281x);
int 		ws281x_deinit				(ws281x_t *ws281x);
int 		ws281x_devices_map			(ws281x_devices_t *devices);
void 		ws281x_devices_unmap 		(ws281x_devices_t *devices);
pwm_fifo_t 	*ws281x_pwmfifo_init		(ws281x_t *ws281x);
void 		ws281x_pwmfifo_deinit		(ws281x_t *ws281x);
void		ws281x_pwmfifo_set			(ws281x_t *ws281x);
void		ws281x_dma_init				(ws281x_devices_t *devices);
void		ws281x_dma_deint			(ws281x_devices_t *devices);

