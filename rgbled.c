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
#include "rgbled_driver.h"
#include "headers/general.h"
#include "headers/rgbled.h"

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

int rgbled_getdataptr(int file_desc, u_int32_t **dma_data_ptr){
    ioctl(file_desc, IOCTL_RGBLED_GETDATAADDR, dma_data_ptr);

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


int rgbled_render(int file_desc){
    ioctl(file_desc, IOCTL_RGBLED_RENDER);

    return 0;
}



int ws281x_init (ws281x_t *ws281x) {

/*
if (ws281x->lednumber == 0 || ws281x->lednumber > 5000 )
  return ERR_LEDNUMBER ;

ws281x->ledarray = (led_t *) calloc (0, sizeof(led_t) * ws281x->lednumber );

if (!ws281x->ledarray)
  return ERR_MEMALLOC ;

ws281x->ledarray->r = 0xff;
ws281x->ledarray->g = 0x0f;
ws281x->ledarray->b = 0xf0;
u_int32_t list[3];


printf("0x%02x%02x%02x%02x \n", ws281x->ledarray->d, ws281x->ledarray->r, ws281x->ledarray->g, ws281x->ledarray->b);
printf("0x%08x \n",  *((u_int32_t *)(ws281x->ledarray)));
printf("VIRTUAL : %p\n",ws281x->ledarray);
printf("PHYSIC  : 0x%08x\n\n",ws281x_convert_virtual(ws281x->ledarray));

printf("VIRTUAL : %p\n",&list);
printf("PHYSIC  : 0x%08x\n",ws281x_convert_virtual(&list));


*/


   int file = open("/dev/"DEV_NAME,0);
	if (file < 0) {
		printf("Can't open device file: %s\n", DEV_NAME);
		exit(-1);
	}

u_int32_t *dma_data = NULL; 
u_int8_t dmach = 0;
u_int8_t ledtype = DEV_TYPE_WS281x;
u_int16_t lednum = 3;
u_int32_t *dataptr;

u_int32_t list[3];
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

    rgbled_setdmachannel(file,&dmach);
    rgbled_setlednumber(file,&l1);
    rgbled_setrgbledtype(file,&ledtype);

    //rgbled_getdataptr(file,&dataptr);

    printf("Pointer %p  :: Value %08x \n",dataptr, *dataptr);
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
