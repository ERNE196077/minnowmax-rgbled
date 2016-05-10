#ifndef DMA_H_
#define DMA_H_
#endif /* DMA_H_ */

#include <linux/ioctl.h>


#define MAJOR_NUM 110
#define IOCTL_REQDMAMEM      _IOWR(MAJOR_NUM , 0 , __u32 )
#define IOCTL_RELDMAMEM      _IO(MAJOR_NUM , 1 )
#define IOCTL_PRINTDMAMEM    _IOR(MAJOR_NUM , 2 , __u32 )


#define DEV_NAME "ws281xdmadev"
