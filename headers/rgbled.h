#ifndef RGBLED_H_
#define RGBLED_H_
#endif /* RGBLED_H_ */

#include "gpio.h"
#include "dma.h"
#include "spi.h"
#include "general.h"

typedef struct{
struct  pci_dev        	*pdev;
		__u32           rgbled_numleds;
		__u8            rgbled_type;
		gpio_dev_t		gpio_dev;
		dma_dev_t		dma_dev;
		spi_dev_t		spi_dev;
} devices_t;