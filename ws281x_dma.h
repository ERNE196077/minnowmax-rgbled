#ifndef DMA_H_
#define DMA_H_
#endif /* DMA_H_ */

#include <linux/ioctl.h>


#define MAJOR_NUM 110
#define DEV_NAME "minnowleddev"

/********  RGBLED FUNCTIONS   ********/
#define IOCTL_RGBLED_SETLEDNUMBER       _IOW(MAJOR_NUM , 0 , __u16 )
#define IOCTL_RGBLED_SETDMACHANNEL      _IOW(MAJOR_NUM , 1 , __u8 )
#define IOCTL_RGBLED_SETRGBLEDTYPE      _IOW(MAJOR_NUM , 2 , __u8 )
#define IOCTL_RGBLED_CONFIGURE          _IO(MAJOR_NUM , 3 )
#define IOCTL_RGBLED_DECONFIGURE        _IO(MAJOR_NUM , 4 )
#define IOCTL_RGBLED_RENDER             _IO(MAJOR_NUM , 5 )

/********  DMA FUNCTIONS   ********/
#define IOCTL_DMA_ADDITEM               _IOW(MAJOR_NUM , 6 , __u32 * )
#define IOCTL_DMA_PRINTITEMS            _IO(MAJOR_NUM , 7 )



struct devices_t ;
