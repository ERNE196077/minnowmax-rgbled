#ifndef RGBLED_DEV_H_
#define RGBLED_DEV_H_

#include "gpio.h"
#include "dma.h"
#include "spi.h"

#define RGBLED_DATA_SIZE_WS281X(lednum)    (sizeof(led_ws281x_t) * lednum )
#define RGBLED_DATA_SIZE_APA102(lednum)    (sizeof(led_apa102_t) * lednum )

 /*****			RGBLEDS STRUCTURES			*****/
typedef struct {
    u_int8_t d;     //dumy variable for 32bit DMA transfer
	u_int8_t r;
	u_int8_t g;
	u_int8_t b;
}__attribute__((packed)) led_apa102_t ;

typedef struct {
    u_int32_t r;
	u_int32_t g;
	u_int32_t b;
}__attribute__((packed)) led_ws281x_t ;

typedef struct{
struct  pci_dev        	*pdev;
		rgbled_conf_t 	rgbled_config;
		gpio_dev_t		gpio_dev;
		dma_dev_t		dma_dev;
		spi_dev_t		spi_dev;
} devices_t;


#endif