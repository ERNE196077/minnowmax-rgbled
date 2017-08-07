/*
 * rgbled.c
 *
 * WS281x Functionality
 * The basic functionality is to set the clock of the PWM as close as possible
 * to the frequency specified in the WS281x documentation.
 * A FIFO is created to contain the registers of the PWM_ON_TIME_DIVISOR which
 * will be varying to draw the logical bits in the PWM signal.
 *
 * My assumption is that the PWM Controller contains a FIFO which will be fill
 * with the data from the DMA transfer.
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h> 
#include "headers/rgbled.h"
#include "headers/rgbled_x11.h"


#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8

static int file;
static int delay;
static led_t *x11_leds; 

/* Ctrl+C callback for user request exit */
void intHandler (int dummy){
	printf("rgbled: Exit requested by user!\n");
	if (!x11_leds){
		x11rgbleds_close();7
		free(x11_leds);
	}
	close(file);	
	exit(0);
}


void rgbled_setconfig(rgbled_conf_t *rgbled_conf){
    ioctl(file, IOCTL_RGBLED_SETCONFIG, rgbled_conf);
}

void rgbled_sendleds(led_t *user_matrix){
    ioctl(file, IOCTL_RGBLED_USERLEDS, user_matrix);
    usleep(delay);
}

void rgbled_function(led_function_t led_function){
    ioctl(file, IOCTL_RGBLED_FUNCTION, &led_function);
    usleep(delay);
}

void rgbled_setcolor(led_t *color){
    ioctl(file, IOCTL_RGBLED_USERCOLOR, color);
    usleep(delay);
}


int rgbled_init (rgbled_conf_t *rgbled_conf) {
  file = open("/dev/"DEV_NAME,0);
  if (file < 0) {
    	printf("Can't open device file: %s\n", DEV_NAME);
    	return 1;
  }
  
  /* Set the time delay to avoid SPI collisions - TODO : Calculate with number of LEDs */
  delay = 8500;
  rgbled_setconfig(file, rgbled_conf);
  return 0;
}

void rgbled_test (void){
  rgbled_function(file,leds_red);
  sleep(1);
  rgbled_function(file,leds_blue);
  sleep(1);
  rgbled_function(file,leds_green);
  sleep(1);
  rgbled_function(file,leds_solar);
  sleep(1);
  rgbled_function(file,leds_warm);
  sleep(1);
  rgbled_function(file,leds_on);
  sleep(1);
  rgbled_function(file,leds_off);
}

int rgbled_x11 (int top, int right, int bottom, int left, int wborder, int hborder){
  int total;
  
  /* Register callback in case an user exit */
  signal(SIGINT, intHandler);

  /* Get the total of LEDs to use */
  total = top + right + left + bottom;

  /* Allocate space for the colors sent to the module */
  x11_leds = (led_t *)malloc(total * sizeof(led_t));

  /* Set function as user defined */
  rgbled_function(file,leds_userdefined);

  /* Initialize X11 pixel matrix */
  x11rgbleds_init(top, right, bottom, left, wborder, hborder, x11_leds);

  /* Start infinite loop */
  while (1){
  	x11rgbleds_query();
  	rgbled_sendleds(file,x11_leds);
  }

  /* In case something happens rollback */
  x11rgbleds_close();
  free(x11_leds);

  return 0;
}
