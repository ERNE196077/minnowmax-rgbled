#ifndef RGBLED_X11_H_
#define RGBLED_X11_H_

#include <stdint.h>
#include <stdlib.h>

#include "../rgbled_driver.h"


typedef struct{
	uint16_t x;
	uint16_t y;
} x11rawpixel_t;

int getRootWindowSize(int *w, int *h);
int x11rgbleds_init(int topleds, int leftleds, int rightleds, int bottomleds, int border, led_t *pleds );

int x11rgbleds_query( void );

int x11rgbleds_close( void );

#endif