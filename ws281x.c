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
#include "ws281x_dma.h"
#include "headers/general.h"
#include "headers/ws281x.h"


static u_int32_t ws281x_convert_virtual	(const volatile void *addr) {
	char filename[40];
	u_int64_t page_frame_buffer;
	int file_descriptor;

	sprintf(filename, "/proc/%d/pagemap", getpid());

	file_descriptor = open(filename, O_RDONLY);							// Open the pagemap of the process.
	if (file_descriptor < 0) {
		perror("Cannot open pagemap file.");
		return (u_int32_t)~0UL;
	}

	u_int32_t offset = ((intptr_t) addr /  BLOCK_SIZE_T) * 8 ;			// Formule to get the offset inside the pagemap.
	u_int32_t reminder = (intptr_t)addr % BLOCK_SIZE_T;					// Reminder of the virtual address, will be added to the physical addres to point the memory.


	if (lseek(file_descriptor, offset, SEEK_SET) != offset) {
		perror("Failed lseek of the current offset.");
		close(file_descriptor);
		return (u_int32_t)~0UL;
	}

	if (read(file_descriptor, &page_frame_buffer, sizeof(page_frame_buffer)) != sizeof(page_frame_buffer)) {
		perror("addr_to_bus() read() failed");
		close(file_descriptor);
		return (u_int32_t)~0UL;
	}

	close(file_descriptor);
	//printf("BEFORE STRIP:\t%" PRIx64 "\n", page_frame_buffer);

	page_frame_buffer &= 0x7fffffffffffff; 									// We are only interested in the first 54 bits.
	page_frame_buffer <<= 12;												// Need to move bits left 12 times to get the correct boundary.
	page_frame_buffer += reminder;											// We add the reminder offset.
	//printf("AFTER STRIP:\t%" PRIx64 "\n", page_frame_buffer);
	return (u_int32_t) page_frame_buffer;
}


int rgbled_setlednumber(int file_desc, u_int16_t *led_number){
    ioctl(file_desc, IOCTL_RGBLED_SETLEDNUMBER, led_number);

    return 0;
}

int rgbled_setdmachannel(int file_desc, u_int8_t *dma_ch_num){
    ioctl(file_desc, IOCTL_RGBLED_SETDMACHANNEL, dma_ch_num);

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


u_int8_t dmach = 0;
u_int8_t ledtype = 0;
u_int16_t lednum = 3;
u_int32_t list[3];
list[0] = 0x00111111;
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
    rgbled_setlednumber(file,&lednum);
    rgbled_setrgbledtype(file,&ledtype);

    rgbled_configure(file);
    //rgbled_dmaprintitems(file);
    //rgbled_dmaadditem(file,&l1);
    //rgbled_dmaadditem(file,&l2);
    //rgbled_dmaadditem(file,&l3);
    //rgbled_dmaadditem(file,&l3);
    //rgbled_dmaprintitems(file);

    //rgbled_render(file);
    //usleep(10000);
    //rgbled_dmaprintitems(file);
    rgbled_deconfigure(file);

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
