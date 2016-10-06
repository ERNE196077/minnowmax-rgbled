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
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
 
#include "headers/rgbled.h"


#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8

static uint32_t ws281x_convert_virtual	(const volatile void *addr) {

	FILE *pagemap = fopen("/proc/self/pagemap", "rb");

  // Seek to the page that the buffer is on
  unsigned long offset = (unsigned long)addr / getpagesize() * PAGEMAP_LENGTH;
  if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
     fprintf(stderr, "Failed to seek pagemap to proper location\n");
     exit(1);
  }
  uint32_t reminder = (intptr_t)addr % BLOCK_SIZE_T;

  // The page frame number is in bits 0-54 so read the first 7 bytes and clear the 55th bit
  unsigned long page_frame_number = 0;
  fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);

  page_frame_number &= 0x7FFFFFFFFFFFFF;
  page_frame_number <<= 12;												// Need to move bits left 12 times to get the correct boundary.
	page_frame_number += reminder;											// We add the reminder offset.

  fclose(pagemap);

	return (uint32_t) page_frame_number;
}


int rgbled_setconfig(int file_desc, rgbled_conf_t *rgbled_conf){
    ioctl(file_desc, IOCTL_RGBLED_SETCONFIG, rgbled_conf);

    return 0;
}


int rgbled_configure(int file_desc){
    ioctl(file_desc, IOCTL_RGBLED_CONFIGURE);

    return 0;
}

int rgbled_deconfigure(int file_desc){
    ioctl(file_desc, IOCTL_RGBLED_DECONFIGURE);

    return 0;
}


int rgbled_render(int file_desc){
    ioctl(file_desc, IOCTL_RGBLED_RENDER);

    return 0;
}

int rgbled_dmaprintdata(int file_desc){
    ioctl(file_desc, IOCTL_DMA_PRINTITEMS);

    return 0;
}


int rgbled_init (rgbled_conf_t *rgbled_conf) {

  int file = open("/dev/"DEV_NAME,0);
    if (file < 0) {
	 printf("Can't open device file: %s\n", DEV_NAME);
	 exit(-1);
  }

  rgbled_setconfig(file, rgbled_conf);

   /* rgbled_deconfigure(file);

//    rgbled_setahblayer(file, &ahbl);

    rgbled_configure(file);
    usleep(10000);

    rgbled_dmaadditem(file,&l1);
    usleep(10000);
    rgbled_dmaadditem(file,&l2);
    usleep(10000);

    rgbled_dmaprintitems(file);

   rgbled_render(file);
   //usleep(100000);
   rgbled_dmaprintitems(file);

*/



char ch;
ch = getchar();

	return 0;
}
