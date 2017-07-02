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
#include "rgbled_x11.h"

/*** RGBLED functions ***/

int rgbled_init (rgbled_conf_t  *rgbled_conf);
void rgbled_test (void);
int rgbled_x11 (void);


#endif /* RGBLED_H_ */

