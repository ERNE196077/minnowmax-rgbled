

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/errno.h>
#include <asm/uaccess.h>
#include "ws281x_dma.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ernestrc");
MODULE_DESCRIPTION("MinnowboardMAX DMA struct memory allocation module.")
MODULE_SUPPORTED_DEVICE("MinnowboardMAX");



static int open = 0 ;

__u32 *ptr ;
__u32 alloc_size = 0;






static int device_open(struct inode *inode, struct file *file){
    if (open)
        return -EBUSY;

    printk(KERN_INFO       "FILE ws281xdmadev OPEN!\n");

    open++;
    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file){
    open--;
    module_put(THIS_MODULE);

    printk(KERN_INFO"FILE ws281xdmadev CLOSE!\n");

    return 0;
}




static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_params){
    switch (ioctl_num){
    case IOCTL_PRINTDMAMEM:
        if(*ptr > 0){
            put_user((__u32)(*ptr), (__u32 *)ioctl_params);
            printk(KERN_INFO"PRINT DMA MEM PUT USER IF :%d\n",(__u32)(*ptr));
        }
        else{
            put_user(0, (__u32 *)ioctl_params);       // When not initialized/error return 0;
            printk(KERN_INFO"PRINT DMA MEM PUT USER ELSE :%d\n",0);
            }
        break;

    case IOCTL_RELDMAMEM:
        kfree(ptr);
        ptr = 0;
        printk(KERN_INFO"RELEASE DMA MEM \n");
        break;

    case IOCTL_REQDMAMEM:
        if(*ptr > 0){
            put_user(0, (__u32 *)ioctl_params);       // When mem is already initialized return 0;
            printk(KERN_INFO"REQUEST DMA MEM PUT USER IF :%d\n",0);
        }
        else {
            get_user (alloc_size , (__u32 *)ioctl_params);
            ptr = kmalloc(alloc_size,GFP_KERNEL | __GFP_DMA);
            put_user ((unsigned long)ptr, (__u32 *)ioctl_params);
            printk(KERN_INFO"REQUEST DMA MEM PUT USER ELSE GET USER: %d\n",alloc_size);
            printk(KERN_INFO"REQUEST DMA MEM PUT USER ELSE SET USER: %p\n",ptr);
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

int reg_dev;

printk(KERN_INFO"Hello World.\n");

reg_dev = register_chrdev(MAJOR_NUM, DEV_NAME, &fop);
if (reg_dev < 0) {
 printk(KERN_INFO "%s failed with %d\n",
 "Sorry, registering the character device ", reg_dev);
 return reg_dev;
 }


return 0;
}

void __exit cleanup_module(){

kfree(ptr);
unregister_chrdev(MAJOR_NUM, DEV_NAME);

printk(KERN_INFO"DIJE\n");

}
