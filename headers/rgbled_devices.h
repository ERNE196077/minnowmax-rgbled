#ifndef RGBLED_DEV_H_
#define RGBLED_DEV_H_

#include "gpio.h"
#include "dma.h"
#include "spi.h"

#define RGBLED_DATA_SIZE_WS281X(lednum)    (sizeof(led_ws281x_t) * lednum + 2 )
#define RGBLED_DATA_SIZE_APA102(lednum)    (sizeof(led_apa102_t) * lednum )


extern uint32_t dma_channels[8];
extern uint32_t gpio_pins[];

 /*****			RGBLEDS STRUCTURES			*****/
typedef struct {
    __u8 dummy;    //dumy variable to fit 32bit.
    __u8 r;
    __u8 g;
    __u8 b;
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
struct  pci_dev        	*pdev;
		__u32 			rgbled_config;
		gpio_dev_t		gpio_dev;
		dma_dev_t		dma_dev;
		spi_dev_t		spi_dev;
} devices_t;


void rgbled_test(__u32 *data, __u32 test_var);

#endif