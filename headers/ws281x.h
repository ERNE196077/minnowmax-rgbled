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



/*****			WS281X DEFAULT VALUES			*****/

#define WS281X_DEFAULT_GPIO_PIN			24		// PWM Capable pins: 22 & 24
#define WS281X_DEFAULT_LED_NUMBER		1
#define WS281X_DEFAULT_DMA_CHANNEL		0
#define WS281X_PWM_REG_HIGH				0xC00848A0
#define WS281X_PWM_REG_LOW				0xC0084850


/*****			RGBLED ERROR CODES			*****/

 #define ERR_LEDNUMBER		0x1		// 	INVALID NUMBER OF
 #define ERR_MEMALLOC		0x2		//	MEMORY ALLOCATION FOR LED DATA NOT SUCCESSFUL
 #define ERR_DMANOTAVAIL	0x3		//	THE CHOOSEN DMA CHANNEL IS IN USE.



 /*****			WS281X STRUCTURES			*****/
typedef struct {
    u_int8_t d;     //dumy variable for 32bit DMA transfer
	u_int8_t r;
	u_int8_t g;
	u_int8_t b;
}led_t;


struct ws281x_devices;

typedef struct
{
	u_int8_t				lednumber;
	led_t					*ledarray;
	u_int8_t				gpio_pin_number;
	u_int8_t				dma_ch_number;
	struct ws281x_devices 	*devices;

}ws281x_t;


/*****			WS281X FUNCTIONS			*****/



int	ws281x_init	(ws281x_t  *ws281x);


