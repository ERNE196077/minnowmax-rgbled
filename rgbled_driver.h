#ifndef RGBLED_DRV_H_
#define RGBLED_DRV_H_

#include <linux/ioctl.h>
#include "headers/gpio.h"
#include "headers/dma.h"
#include "headers/spi.h"
#include "headers/general.h"


#define MAJOR_NUM 121
#define DEV_NAME "rgbled_device"

#define DEV_TYPE_WS281x 0
#define DEV_TYPE_APA102 1

/********  RGBLED FUNCTIONS   ********/
#define IOCTL_RGBLED_SETLEDNUMBER       _IOW(MAJOR_NUM , 0 , __u32 )
#define IOCTL_RGBLED_SETDMACHANNEL      _IOW(MAJOR_NUM , 1 , __u8 )
#define IOCTL_RGBLED_SETRGBLEDTYPE      _IOW(MAJOR_NUM , 2 , __u8 )
#define IOCTL_RGBLED_GETDATAADDR	    _IOR(MAJOR_NUM , 8 , __u32 *)
#define IOCTL_RGBLED_CONFIGURE          _IO(MAJOR_NUM , 3 )
#define IOCTL_RGBLED_DECONFIGURE        _IO(MAJOR_NUM , 4 )
#define IOCTL_RGBLED_RENDER             _IO(MAJOR_NUM , 5 )

/********  DMA FUNCTIONS   ********/
#define IOCTL_DMA_PRINTITEMS            _IO(MAJOR_NUM , 7 )


typedef struct{
struct  pci_dev        	*pdev;
		__u32           rgbled_numleds;
		__u8            rgbled_type;
		gpio_dev_t		gpio_dev;
		dma_dev_t		dma_dev;
		spi_dev_t		spi_dev;
} devices_t;




#endif /* RGBLED_DRV_H_ */