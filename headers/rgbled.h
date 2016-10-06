#ifndef RGBLED_H_
#define RGBLED_H_


/*
 * rgbled.h
 *
 * Adding needed structures and definitions to handle the ws281x LED's.
 *
 *  Created on: Jul 30, 2015
 *      Author: minnow
 */

 
#include "../rgbled_driver.h"

/*****			WS281X DEFAULT VALUES			*****/

#define WS281X_DEFAULT_LED_NUMBER		1
#define WS281X_DEFAULT_DMA_CHANNEL		0


/*****			RGBLED FUNCTIONS			*****/

int	rgbled_init	(rgbled_conf_t  *rgbled_conf);


#endif /* RGBLED_H_ */

