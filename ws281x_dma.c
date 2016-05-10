

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/errno.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>
#include "ws281x_dma.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ernestrc");
MODULE_DESCRIPTION("MinnowboardMAX DMA struct memory allocation module.")
MODULE_SUPPORTED_DEVICE("MinnowboardMAX");



static int open = 0 ;
static dev_t first;         // Global variable for the first device number
static struct cdev c_dev;   // Global variable for the character device structure
static struct class *cl;    // Global variable for the device class

__u32 *ptr ;
__u32 alloc_size = 0;






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




static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_params){
    switch (ioctl_num){
    case IOCTL_PRINTDMAMEM:
        if(ptr > 0){
            put_user((__u32)(*ptr), (__u32 *)ioctl_params);
            printk(KERN_INFO"DMA first register: 0x%d\n",(__u32)(*ptr));
        }
        else{
            put_user(0, (__u32 *)ioctl_params);       // When not initialized/error return 0;
            printk(KERN_INFO"DMA memory not allocated. Cannot print register\n");
            }

        break;

    case IOCTL_RELDMAMEM:
        kfree(ptr);
        ptr = 0;
        printk(KERN_INFO"DMA memory released!\n");
        break;

    case IOCTL_REQDMAMEM:
        get_user (alloc_size , (__u32 *)ioctl_params);
        if(ptr > 0){
            put_user(0, (__u32 *)ioctl_params);       // When mem is already initialized return 0;
            printk(KERN_INFO"DMA request failed! DMA memory already allocated in %08x\n",*ptr);
        }
        else {

            ptr = kmalloc(alloc_size,GFP_KERNEL | __GFP_DMA);
            put_user ((unsigned long)ptr, (__u32 *)ioctl_params);
            printk(KERN_INFO"DMA memory requested, %d bytes in %08x allocated\n",alloc_size,*ptr);
        }
        break;
    }
    return 0;
}



struct file_operations fop = {
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release

};



int __init init_module(){

printk(KERN_INFO"##############################Hello World.##############################\n");
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

kfree(ptr);
device_destroy(cl, first);
class_destroy(cl);
unregister_chrdev_region(first, 1);
printk(KERN_INFO"#########################DIJE##############################\n");

}
