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
 
#include "headers/rgbled.h"
#include "headers/rgbled_x11.h"


#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8


//static led_t test_led = {2,0,20};
static int file;
static int w, h;
void rgbled_setconfig(int file_desc, rgbled_conf_t *rgbled_conf){
    ioctl(file_desc, IOCTL_RGBLED_SETCONFIG, rgbled_conf);
}

void rgbled_sendleds(int file_desc, led_t *user_matrix){
    ioctl(file_desc, IOCTL_RGBLED_USERLEDS, user_matrix);
    usleep(8500);
}

void rgbled_function(int file_desc, led_function_t led_function){
    ioctl(file_desc, IOCTL_RGBLED_FUNCTION, &led_function);
    usleep(8500);
}

void rgbled_setcolor(int file_desc, led_t *color){
    ioctl(file_desc, IOCTL_RGBLED_USERCOLOR, color);
    usleep(8500);
}

int rgbled_init (rgbled_conf_t *rgbled_conf) {
  file = open("/dev/"DEV_NAME,0);
  if (file < 0) {
    	printf("Can't open device file: %s\n", DEV_NAME);
    	return 1;
  }
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


int rgbled_x11 (void){
  getScreenSize(&w, &h);

  printf("Screen Width: %d  x  Height: %d \n",w ,h);

  return 0;
}