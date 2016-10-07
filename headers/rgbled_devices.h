#ifndef RGBLED_DEV_H_
#define RGBLED_DEV_H_

#include "gpio.h"
#include "dma.h"
#include "spi.h"

#define RGBLED_DATA_SIZE_WS281X(lednum)    (sizeof(led_ws281x_t) * lednum )
#define RGBLED_DATA_SIZE_APA102(lednum)    (sizeof(led_apa102_t) * lednum )

 /*****			RGBLEDS STRUCTURES			*****/
typedef struct {
    uint8_t d;    //dumy variable to fit 32bit.
    uint8_t r;
    uint8_t g;
    uint8_t b;
}__attribute__((packed)) led_apa102_t ;

typedef struct {
    uint32_t r;
	uint32_t g;
	uint32_t b;
	#define RGBLEDWS281X_LOW  				0x6
	#define RGBLEDWS281X_HIGH				0x4
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
		rgbled_conf_t 	rgbled_config;
		gpio_dev_t		gpio_dev;
		dma_dev_t		dma_dev;
		spi_dev_t		spi_dev;
} devices_t;


void rgbled_test(void *data);

#endif