/*
 * ws281x.c
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
#include "rgbled_driver.h"
#include "headers/general.h"
#include "headers/ws281x.h"

#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8

static u_int32_t ws281x_convert_virtual	(const volatile void *addr) {

	FILE *pagemap = fopen("/proc/self/pagemap", "rb");

   // Seek to the page that the buffer is on
   unsigned long offset = (unsigned long)addr / getpagesize() * PAGEMAP_LENGTH;
   if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
      fprintf(stderr, "Failed to seek pagemap to proper location\n");
      exit(1);
   }
   u_int32_t reminder = (intptr_t)addr % BLOCK_SIZE_T;

   // The page frame number is in bits 0-54 so read the first 7 bytes and clear the 55th bit
   unsigned long page_frame_number = 0;
   fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);

   page_frame_number &= 0x7FFFFFFFFFFFFF;
   page_frame_number <<= 12;												// Need to move bits left 12 times to get the correct boundary.
	page_frame_number += reminder;											// We add the reminder offset.


   fclose(pagemap);



	return (u_int32_t) page_frame_number;
}


int rgbled_setlednumber(int file_desc, u_int32_t *led_number){
    ioctl(file_desc, IOCTL_RGBLED_SETLEDNUMBER, led_number);

    return 0;
}

int rgbled_setdmachannel(int file_desc, u_int8_t *dma_ch_num){
    ioctl(file_desc, IOCTL_RGBLED_SETDMACHANNEL, dma_ch_num);

    return 0;
}

int rgbled_setahblayer(int file_desc, u_int8_t *dma_ahb_layer){
    ioctl(file_desc, IOCTL_RGBLED_SETAHBLAYER, dma_ahb_layer);

    return 0;
}

int rgbled_setrgbledtype(int file_desc, u_int8_t *led_type){
    ioctl(file_desc, IOCTL_RGBLED_SETRGBLEDTYPE, led_type);

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

int rgbled_dmaadditem(int file_desc,u_int32_t *addr){
    ioctl(file_desc, IOCTL_DMA_ADDITEM, addr);

    return 0;
}

int rgbled_dmaprintitems(int file_desc){
    ioctl(file_desc, IOCTL_DMA_PRINTITEMS);

    return 0;
}

int rgbled_render(int file_desc){
    ioctl(file_desc, IOCTL_RGBLED_RENDER);

    return 0;
}



int ws281x_init (ws281x_t *ws281x) {

   int file = open("/dev/"DEV_NAME,0);
	if (file < 0) {
		printf("Can't open device file: %s\n", DEV_NAME);
		exit(-1);
	}

u_int8_t ahbl = 3;
u_int8_t dmach = 0;
u_int8_t ledtype = 0;
u_int16_t lednum = 3;
u_int32_t list[3];
u_int32_t tmp_var = 0xAf00ff00;
list[0] = 0x100F1111;
list[1] = 0x11001111;
list[2] = 0x11110011;
u_int32_t l1, l2, l3;
l1 = ws281x_convert_virtual(&list[0]);
l2 = ws281x_convert_virtual(&list[1]);
l3 = ws281x_convert_virtual(&list[2]);
printf("&list[0] : %08x   %p\n",l1,&list[0]);
printf("&list[1] : %08x   %p\n",l2,&list[1]);
printf("&list[2] : %08x   %p\n",l3,&list[2]);
    //rgbled_deconfigure(file);

    rgbled_setdmachannel(file,&dmach);
    rgbled_setlednumber(file,&l1);
    rgbled_setrgbledtype(file,&ledtype);

    rgbled_deconfigure(file);

//   0
    rgbled_configure(file);
    //rgbled_dmaprintitems(file);
    usleep(10000);
    rgbled_dmaadditem(file,&l1);
    usleep(10000);
    rgbled_dmaadditem(file,&l2);
    usleep(10000);
rgbled_dmaadditem(file,&l1);
    usleep(10000);
    rgbled_dmaadditem(file,&l2);
    usleep(10000);
rgbled_dmaadditem(file,&l1);
    usleep(10000);

    rgbled_dmaprintitems(file);

   rgbled_render(file);
    usleep(100000);
    rgbled_dmaprintitems(file);



/*
	 * THIS IS FOR PWM. ABOUT TO DELETE. NEED TO BACKUP, LIGHT BULB DIMMER CONTROLS
	 *

	GPIO_CFG_FUNCTION(devices->gpio_pin->__cfg__,1)

	while (1){
	for (int i = 0x10 ; i < 0xff ; i++ ){
		devices->pwm_dev->__pwmctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_ON_TIME_DIVISOR(i) | 	PWM_BASE_UNIT(0x0002);
		usleep(10000);
	}
	for (int i = 0xff ; i > 0x10 ; i-- ){
		devices->pwm_dev->__pwmctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_ON_TIME_DIVISOR(i) | PWM_BASE_UNIT(0x0002);
		usleep(10000);
	}
	}
	*/
	//ws281x_spi_stop(ws281x);

	//ws281x_dma_stop(ws281x);
	//usleep(100);
	//ws281x_print_registers(ws281x);
	//ws281x_fifo_init(ws281x);

	//ws281x_gpio_setup(ws281x);

	//ws281x_spi_setup(ws281x);
	//ws281x_print_registers(ws281x);


	//ws281x_spi_setup(ws281x);
	//ws281x_spi_start(ws281x);


	//ws281x_dma_setup(ws281x);
	//ws281x_print_registers(ws281x);

	//ws281x_dma_start(ws281x);
	//ws281x_spi_additems(ws281x);

	//usleep(100);

	//ws281x_print_registers(ws281x);



	return 0;
}
