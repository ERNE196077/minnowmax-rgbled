 #ifndef RGBLED_DEV_H_
#define RGBLED_DEV_H_

#include "gpio.h"
#include "dma.h"
#include "spi.h"
#include "../rgbled_driver.h"

 /*****			RGBLEDS STRUCTURES			*****/
typedef struct {
	/* global: 3 bits = 1 ; 5 bits = brightness 0 - 31 */
    __u8 r;
    __u8 g;
    __u8 b;
    __u8 global;	
}__attribute__((packed)) led_apa102_t ;

typedef struct {
    __u32 g;
	__u32 r;
	__u32 b;
	#define RGBLEDWS281X_LOW  				0x4
	#define RGBLEDWS281X_HIGH				0x6
	#define RGBLEDWS281X_RGBCONV(value) 	(value & 0x80 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 21 | \
											(value & 0x40 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 18 | \
											(value & 0x20 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 15 | \
											(value & 0x10 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 12 | \
											(value & 0x8 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 9 | \
											(value & 0x4 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 6 | \
											(value & 0x2 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) << 3 | \
											(value & 0x1 ? RGBLEDWS281X_HIGH : RGBLEDWS281X_LOW) 

}__attribute__((packed)) led_ws281x_t ;

typedef struct{
struct  pci_dev        	*pdev_dma;
struct  pci_dev        	*pdev_spi;
		__u32 			rgbled_config;
		gpio_dev_t		gpio_dev;
		dma_dev_t		dma_dev;
		spi_dev_t		spi_dev;
} devices_t;



extern uint32_t dma_channels[8];
extern uint32_t gpio_pins[];
extern devices_t devices;


void rgbled_setcolor(__u32 *data, led_t ledcolor);
void rgbled_usermatrix(__u32 *data, led_t *ledmatrix);

#endif