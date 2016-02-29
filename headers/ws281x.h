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

//#include "headers/dma.h"
//#include "headers/gpio.h"
//#include "headers/pwm.h"


/*****			WS281X DEFAULT VALUES			*****/

#define WS281X_DEFAULT_GPIO_PIN			24		// PWM Capable pins: 22 & 24
#define WS281X_DEFAULT_LED_NUMBER		1		
#define WS281X_DEFAULT_DMA_CHANNEL		0		
#define WS281X_PWM_REG_HIGH				0xC00848A0
#define WS281X_PWM_REG_LOW				0xC0084850


 /*****			WS281X STRUCTURES			*****/
typedef struct {
	u_int8_t r;
	u_int8_t g;
	u_int8_t b;
}led_t;


struct ws281x_devices;

typedef struct
{
	u_int8_t			lednumber:
	led_t				ledarray[lednumber];
	u_int8_t			gpio_pinnumber;
	u_int8_t			dma_channel;
	ws281x_devices 		*devices;
	
}ws281x_t;


/*****			WS281X FUNCTIONS			*****/



int 		ws281x_init					(ws281x_t *ws281x);
int 		ws281x_deinit				(ws281x_t *ws281x);
int 		ws281x_devices_map			(ws281x_devices_t *devices);
void 		ws281x_devices_unmap 		(ws281x_devices_t *devices);
int 		ws281x_pwmfifo_init			(ws281x_t *ws281x);
void 		ws281x_pwmfifo_deinit		(ws281x_t *ws281x);
void		ws281x_pwmfifo_set			(ws281x_t *ws281x);
void		ws281x_dma_init				(ws281x_devices_t *devices);
void		ws281x_dma_deint			(ws281x_devices_t *devices);


