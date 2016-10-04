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



/*****			WS281X DEFAULT VALUES			*****/

#define WS281X_DEFAULT_LED_NUMBER		1
#define WS281X_DEFAULT_DMA_CHANNEL		0


 /*****			RGBLEDS STRUCTURES			*****/
typedef struct {
    u_int8_t d;     //dumy variable for 32bit DMA transfer
	u_int8_t r;
	u_int8_t g;
	u_int8_t b;
}__attribute__((packed)) led_apa102_t ;

typedef struct {
    u_int32_t r;
	u_int32_t g;
	u_int32_t b;
}__attribute__((packed)) led_ws281x_t ;

/*		rgbled_conf_t ::  4 bit led type ; 4 bits DMA channel ; 24 bits led number 	*/
typedef u_int32_t rgbled_conf_t;
#define RGBLED_CONF_WS281X			0
#define RGBLED_CONF_APA102 			1
#define RGBLED_CONF_MASK_LEDTYPE	(0xF << 28)
#define RGBLED_CONF_DMACH(value)	(value << 24)
#define RGBLED_CONF_MASK_DMACH		(0xF << 24)
#define	RGBLED_CONF_LEDNUM(value)	(value)
#define RGBLED_CONF_MASK_LEDNUM		(0xFFFFFF)


/*****			RGBLED FUNCTIONS			*****/

int	rgbled_init	(rgbled_conf_t  *rgbled_conf);


#endif /* RGBLED_H_ */

