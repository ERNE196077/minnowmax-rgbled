

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/errno.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>
#include "ws281x_dma.h"
#include "headers/pwm.h"
#include "headers/dma.h"
#include "headers/gpio.h"
#include "headers/spi.h"
#include "headers/general.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ernestrc");
MODULE_DESCRIPTION("MinnowboardMAX APA102/WS281X module.")
MODULE_SUPPORTED_DEVICE("MinnowboardMAX");





/********* DEVICE VARIABLES  ************/
static int open = 0 ;
static dev_t first;         // Global variable for the first device number
static struct cdev c_dev;   // Global variable for the character device structure
static struct class *cl;    // Global variable for the device class

/********* RGBLED VARIABLES  ************/

#define WS281X_DMADESCMEMORY(lednum)    ((sizeof(dma_channel_t) * ((3 * lednum) + 2))  % 4096) > 0 ? \
                                        ((sizeof(dma_channel_t) * ((3 * lednum) + 2))  / 4096)+1)*4096 : \
                                        ((sizeof(dma_channel_t) * ((3 * lednum) + 2))  / 4096)*4096 )

#define APA102_DMADESCMEMORY(lednum)    ( (sizeof(dma_channel_t) * (lednum + 2)) % 4096) > 0 ? \
                                        (((sizeof(dma_channel_t) * (lednum + 2)) / 4096)+1)*4096 : \
                                        ((sizeof(dma_channel_t) * (lednum + 2)) / 4096)*4096 )

typedef struct rgbled_devices {
    volatile pwm_t          *pwm_dev;
    volatile dma_channel_t  *dma_ch;
    volatile dma_channel_t  *dma_descriptors;
    volatile dma_cfg_t      *dma_cfg;
    volatile gpio_t         *gpio_pin_spi_mosi;
    volatile gpio_t         *gpio_pin_spi_clk;
    volatile ssp_control_t  *ssp_control_block;
    volatile ssp_general_t  *ssp_general_block;
} rgbled_devices_t;

static          rgbled_devices 	*devices;
static          __u16           lednumber;
static          __u8	        dma_ch_number;
static          __u8            rgbled_type;
static          __u32           dma_desc_mem;
volatile        __u32           *gpio_base;
volatile        __u32           *spi_base;
volatile        __u32           *dma_base;



/**************************************************************************
***************************************************************************
***************************************************************************
*****************     OPEN CLOSE FUNCTIONS          ***********************
***************************************************************************
***************************************************************************
***************************************************************************/

static int device_open(struct inode *inode, struct file *file){
    if (open)
        return -EBUSY;

    printk(KERN_INFO       "FILE "DEV_NAME" OPEN!\n");

    open++;
    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file){
    open--;
    module_put(THIS_MODULE);

    printk(KERN_INFO"FILE "DEV_NAME" CLOSE!\n");

    return 0;
}

/**************************************************************************
***************************************************************************
***************************************************************************
*****************     IOCTL FUNCTIONS          ****************************
***************************************************************************
***************************************************************************
***************************************************************************/

#define IOCTL_RGBLED_SETLEDNUMBER       _IOw(MAJOR_NUM , 0 , __u16 )
#define IOCTL_RGBLED_SETDMACHANNEL      _IOw(MAJOR_NUM , 1 , __u8 )
#define IOCTL_RGBLED_SETRGBLEDTYPE      _IOw(MAJOR_NUM , 2 , __u8 )
#define IOCTL_RGBLED_CONFIGURE          _IO(MAJOR_NUM , 3 )
#define IOCTL_RGBLED_DECONFIGURE        _IO(MAJOR_NUM , 4 )
#define IOCTL_RGBLED_RENDER             _IO(MAJOR_NUM , 5 )

/********  DMA FUNCTIONS   ********/
#define IOCTL_DMA_ADDITEM               _IOW(MAJOR_NUM , 6 , __u32 )
#define IOCTL_DMA_PRINTITEMS            _IO(MAJOR_NUM , 7 )

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_params){
    switch (ioctl_num){
    case IOCTL_RGBLED_SETLEDNUMBER:
        get_user(lednumber, (__u16 *)ioctl_params );

        break;

    case IOCTL_RGBLED_SETDMACHANNEL:
        get_user(dma_ch_number, (__u8 *)ioctl_params);

        break;

    case IOCTL_RGBLED_SETRGBLEDTYPE:
        get_user(rgbled_type, (__u8 *)ioctl_params);

        break;

    case IOCTL_RGBLED_CONFIGURE:


        if (!(gpio_base = (volatile u_int32_t *) ioremap(GPIO_SCORE_BASE_ADDR, BLOCK_SIZE))) {
            perror("GPIO BLOCK MAPPING FAILED!");
            return -1;
        }

        if (!(dma_base = (volatile u_int32_t *) ioremap(DMA_BASE_ADDR, BLOCK_SIZE))) {
            perror("DMA BLOCK MAPPING FAILED!");
            return -1;
        }

        if (!(spi_base = (volatile u_int32_t *) ioremap(SPI_BAR, BLOCK_SIZE))) {
            perror("SPI BLOCK MAPPING FAILED!");
            return -1;
        }


        devices->dma_descriptors = kmalloc(dma_desc_mem,GFP_KERNEL | GFP_USER);
        devices->gpio_pin_spi_clk = (volatile gpio_t *)(gpio_base + gpio_pins[9]);
        devices->gpio_pin_spi_mosi = (volatile gpio_t *)(gpio_base + gpio_pins[11]);
        devices->ssp_control_block = (volatile ssp_control_t *)spi_base;
        devices->ssp_general_block = (volatile ssp_control_t *)(spi_base + SPI_SSP_GENERAL_OFFSET);
        devices->dma_ch = (volatile dma_channel_t *)(dma_base + dma_channels[dma_ch_number]);
        devices->dma_cfg = (volatile dma_cfg_t *)(dma_base + DMA_DMACCFG_OFF);

        if(rgbled_type = DEV_WS281X)
            dma_desc_mem = WS281X_DMADESCMEMORY(lednumber);
        else
            dma_desc_mem = APA102_DMADESCMEMORY(lednumber);
        devices->dma_descriptors = kmalloc(dma_desc_mem, GFP_KERNEL | __GFP_DMA);



        break;

    case IOCTL_RGBLED_DECONFIGURE:

        break;

    case IOCTL_RGBLED_RENDER:

        break;

    case IOCTL_DMA_ADDITEM:

        break;

    case IOCTL_DMA_PRINTITEMS:

        break;

    }
    return 0;
}


/**************************************************************************
***************************************************************************
***************************************************************************
*****************     INIT CLEANUP FUNCTIONS          *********************
***************************************************************************
***************************************************************************
***************************************************************************/


struct file_operations fop = {
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release

};


int __init init_module(){
    printk(KERN_INFO"############################## WELCOME RGBLED ##############################\n");

    devices = kmalloc(sizeof(rgbled_devices_t),GFP_KERNEL | GFP_USER);



  if (alloc_chrdev_region(&first, 0, 1, "ws281x_dma") < 0)
  {
    return -1;
  }
    if ((cl = class_create(THIS_MODULE, "ws281x_dma")) == NULL)
  {
    unregister_chrdev_region(first, 1);
    return -1;
  }
    if (device_create(cl, NULL, first, NULL, "ws281x_dma") == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }
    cdev_init(&c_dev, &fop);
    if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }
  return 0;



}



void __exit cleanup_module(){

kfree(devices);

device_destroy(cl, first);
class_destroy(cl);
unregister_chrdev_region(first, 1);
printk(KERN_INFO"#########################DIJE##############################\n");

}
