    

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
#include <linux/pci_regs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include "rgbled_driver.h"



MODULE_LICENSE("GPL");
MODULE_AUTHOR("ernestrc");
MODULE_DESCRIPTION("MinnowboardMAX APA102/WS281X module.");
MODULE_SUPPORTED_DEVICE("MinnowboardMAX");

/********* DEVICE VARIABLES  ************/
static int open = 0 ;
static dev_t first;         // Global variable for the first device number
static struct cdev c_dev;   // Global variable for the character device structure
static struct class *cl;    // Global variable for the device class

/********* RGBLED VARIABLES  ************/

#define WS281X_DMADESCMEMORY(lednum)    (sizeof(__u32) * 3 * lednum )
/*NEED TO MODIFY*/   
#define APA102_DMADESCMEMORY(lednum)    (sizeof(__u32) * 3 * lednum )

devices_t    devices;

/*  PCI VARIABLES   */
static      int     err;

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

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_params){
    switch (ioctl_num){
    case IOCTL_RGBLED_SETLEDNUMBER:
        get_user(devices.rgbled_numleds, (__u32 *)ioctl_params );

        break;


    case IOCTL_RGBLED_SETDMACHANNEL:
        get_user(devices.dma_dev.dma_ch_number, (__u8 *)ioctl_params);

        break;
                

    case IOCTL_RGBLED_SETRGBLEDTYPE:
        get_user(devices.rgbled_type, (__u8 *)ioctl_params);

        break;

    case IOCTL_RGBLED_CONFIGURE:


        /************ SETTING DMA MEMORY ************/
        if(devices.rgbled_type == DEV_WS281X)
            devices.dma_dev.dma_data_size = WS281X_DMADESCMEMORY(devices.rgbled_numleds);
        else
            devices.dma_dev.dma_data_size = APA102_DMADESCMEMORY(devices.rgbled_numleds);

        /************ SETTING GPIO ************/
        
        /************ SETTING SPI ************/
        devices.spi_dev.ssp_control_block->__sscr0__ =
            SPI_SSP_SSCR0_MOD_NORMALSSPMODE |
            SPI_SSP_SSCR0_ACS_CLOCKBYAUDIOCLOCK |
            SPI_SSP_SSCR0_TIM_NOINTERRUPTFIFOUNDERRUN |
            SPI_SSP_SSCR0_RIM_NOINTERRUPTFIFOOVERRUN |
            SPI_SSP_SSCR0_NCS_CLOCKBYECS |
            SPI_SSP_SSCR0_EDSS_ONEPREPENDTODSS |
            SPI_SSP_SSCR0_SCR_SERIALCLOCKRATE(20) |
            SPI_SSP_SSCR0_SSE_SSPDISABLE |
            SPI_SSP_SSCR0_ECS_EXTERNALCLOCKFROMGPIO |
            SPI_SSP_SSCR0_FRF_TEXASINSTRUMENTS |
            SPI_SSP_SSCR0_DSS_DATASIZESELECT(0xF) ; //0X7

        devices.spi_dev.ssp_control_block->__sscr1__ =
            SPI_SSP_SSCR1_TTELP_TXDTRISTATEONCLOCKEDGE |
            SPI_SSP_SSCR1_TTE_TXDNOTTRISTATED |
            SPI_SSP_SSCR1_EBCEI_NOINTERRUPTONBITCOUNTERR |
            SPI_SSP_SSCR1_SCFR_SLAVECLKFREERUNNING |
            SPI_SSP_SSCR1_ECRA_NOREQCLOCKFROMOTHERSSP |
            SPI_SSP_SSCR1_ECRB_NOREQCLOCKFROMOTHERSSP |
            SPI_SSP_SSCR1_SCLKDIR_MASTERMODECLOCK |
            SPI_SSP_SSCR1_SFRMDIR_MASTERMODEFRAME |
            SPI_SSP_SSCR1_RWOT_TRANSMITANDRECEIVE |
            SPI_SSP_SSCR1_TRAIL_CPUHANDLETRAILINGBYTE |     // NEED TO CHANGE
            SPI_SSP_SSCR1_TSRE_DMATRANSMITREQDISABLE |      // NEED TO CHANGE
            SPI_SSP_SSCR1_RSRE_DMARECEIVEREQDISABLE |       // NEED TO CHANGE
            SPI_SSP_SSCR1_TINTE_NOINTERRUPTONTIMEOUT |
            SPI_SSP_SSCR1_PINTE_NOTRAILINGBYTEINTERRUPT |
            SPI_SSP_SSCR1_IFS_FRAMEPOLARITYBYTHEFORMAT |
            //SPI_SSP_SSCR1_STRF_RWFORRECEIVEFIFO |
            //SPI_SSP_SSCR1_EFWR_ENABLEFIFOREADWRITE |
            SPI_SSP_SSCR1_RFT_RECEIVEFIFOTRIGTHRESHOLD(0x0) |
            SPI_SSP_SSCR1_TFT_TRANSMITFIFOTRIGTHRESHOLD(0x0) |
            SPI_SSP_SSCR1_TIE_TRANSFIFOLEVELINTERRDISA |    // POSSIBLY CHANGE
            SPI_SSP_SSCR1_RIE_RECEIFIFOLEVELINTERRDISA ;    // POSSIBLY CHANGE

        devices.spi_dev.ssp_control_block->__ssacd__ =
            SPI_SSP_SSACD_ACPS_AUDIOCLK_32_842MHZ |
            SPI_SSP_SSACD_SCDB_SYSCLKNODIVIDED |
            SPI_SSP_SSACD_ACDS_AUDIOCLKDIVIDER(2) ;

        devices.spi_dev.ssp_control_block->__sitf__ &= ~0xFFFF ;

        devices.spi_dev.ssp_control_block->__sitf__ |=
            SPI_SSP_SITF_SETTRANSLOWWATERMARKSPI(0x0) |
            SPI_SSP_SITF_SETTRANSHIGHWATERMARKSPI(0xFF) ;
            
       devices.spi_dev.ssp_general_block->__prv_clock_params__ =
            SPI_SSP_PRVCLKPARAMS_CLOCKUPDATE |
            SPI_SSP_PRVCLKPARAMS_N_DIVISOR(0xFF) |
            SPI_SSP_PRVCLKPARAMS_M_DIVIDEND(0x3) |
            SPI_SSP_PRVCLKPARAMS_ENABLECLOCK ;
   
        
        /************ SETTING DMA ************/
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = 0x1;
        devices.dma_dev.dma_ch->__sar_l__ = 0x0;
        devices.dma_dev.dma_ch->__dar_l__ = SPI_BASE_ADDR+0x10;
        devices.dma_dev.dma_ch->__llp_l__ = 0x0;
        devices.dma_dev.dma_ch->__ctl_l__ =
            DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGENABLE |
            DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE |
            DMA_CTL_LO_SMS_SRCMASTERLAYER1 |
            DMA_CTL_LO_DMS_DSTMASTERLAYER1 |
            DMA_CTL_LO_TTFC_FLOWCONTROLBYDMA |
            DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERDISABLE |
            DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERDISABLE |
            DMA_CTL_LO_SRCMSIZE_SRCBURSTITEMNUM(0x0) |
            DMA_CTL_LO_DESTMSIZE_DSTBURSTITEMNUM(0x0) |
            DMA_CTL_LO_SINC_SOURCEADDRNOCHANGE |
            DMA_CTL_LO_DINC_DESTADDRNOCHANGE |
            DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF32BITS |
            DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF32BITS |
            DMA_CTL_LO_INTEN_INTERRUPTSDISABLED ;
        devices.dma_dev.dma_ch->__ctl_h__ =
            DMA_CTL_HI_DONE_DONEBITZERO |
            DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0x1) ;
        devices.dma_dev.dma_ch->__cfg_l__ =
            DMA_CFG_LO_RELOADDST_NORELOADDSTAFTERTRANSFER |
            DMA_CFG_LO_RELOADSRC_NORELOADSRCAFTERTRANSFER |
            DMA_CFG_LO_MAXABRST_NOLIMITBURSTTRANSFER |
            DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVEHIGH |
            DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVEHIGH |
            DMA_CFG_LO_LOCKB_BUSNOTLOCKED |
            DMA_CFG_LO_LOCKCH_CHANNELNOTLOCKED |
            DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSFER |
            DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSFER |
            DMA_CFG_LO_HSSELSRC_SOURCESWHANDSHAKING |               // TO BE CHANGED PROBABLY
            DMA_CFG_LO_HSSELDST_DESTSWHANDSHAKING |
            DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
            DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY ;
        devices.dma_dev.dma_ch->__cfg_h__ =
            DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(0x0) |
            DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
            DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
            DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
            DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
            DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

        devices.dma_dev.dma_cfg->__dmacfgre_l__ = DMA_DMACFGREG_L_DMA_ENA;

        break;

    case IOCTL_RGBLED_DECONFIGURE:

        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        devices.dma_dev.dma_cfg->__reqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        devices.dma_dev.dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        devices.dma_dev.dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        devices.dma_dev.dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        devices.spi_dev.ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = 0;
        
        break;

    case IOCTL_RGBLED_RENDER:

        devices.spi_dev.ssp_control_block->__sscr0__ |= SPI_SSP_SSCR0_SSE_SSPENABLE;
        
        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);

        msleep(100);
        devices.dma_dev.dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        devices.dma_dev.dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        msleep(100);

        devices.dma_dev.dma_cfg->__reqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        
        devices.dma_dev.dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        
        break;
    case IOCTL_RGBLED_GETDATAADDR:
        put_user(devices.dma_dev.dma_data_ptr, (__u32 **)ioctl_params);

     
    break;

    
    case IOCTL_DMA_PRINTITEMS:
        printk(KERN_ALERT"DMA data address: %p\n\n",devices.dma_dev.dma_data_ptr);
        printk(KERN_ALERT"DMA mem size : %08x\n\n",devices.dma_dev.dma_data_size);
        printk(KERN_INFO"__chenreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__chenreg_l__ );
        printk(KERN_INFO"__dmacfgre_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__dmacfgre_l__ );
        printk(KERN_INFO"__reqdstreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__reqdstreg_l__);
        printk(KERN_INFO"__reqsrcreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__reqsrcreg_l__);
        printk(KERN_INFO"__sglrqdstreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__sglrqdstreg_l__);
        printk(KERN_INFO"__sglrqsrcreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__sglrqsrcreg_l__);

        /* AGREGAR MAS REGISTROS */
    break;

    }
    return 0;
}

/**************************************************************************
***************************************************************************
***************************************************************************
*****************     PCI STUFF          **********************************
***************************************************************************
***************************************************************************
***************************************************************************/

static int pci_rgbled_probe (struct pci_dev *pdev, const struct pci_device_id *id){

    /*  REGISTER PCI DEVICE  */
    err = pci_enable_device(pdev);
    if (err)
        goto err_enable_device;

    err = pci_request_regions(pdev, "rgbled_driver");
    if (err)
        goto err_request_regions;

    err = pci_set_dma_mask(pdev, DMA_BIT_MASK(29));
    if (err)
        goto err_set_dma_mask;

    err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(29));
    if (err)
        goto err_set_dma_mask;

    devices.pdev = pci_dev_get(pdev);
    devices.dma_dev.dma_bar = pci_resource_start (pdev, 0);         /*  GET DMA BASE ADDRESS REGISTER  */
    devices.dma_dev.dma_bar_size = pci_resource_len (pdev, 0);      /*  GET DMA BASE ADDRESS REGISTER SIZE  */
    pci_set_master(pdev);                           /*  ENABLE DEVICE AS DMA  */

    /*  CREATE DMA MEMORY POOL FOR THE DATA  */
    devices.dma_dev.dma_data_ptr = kmalloc(WS281X_DMADESCMEMORY(10),GFP_USER);
    if (devices.dma_dev.dma_data_ptr == NULL){
        err = -1;
        goto err_kmalloc;
    }

    /*  MAP IO MEM FOR GPIO, DMA AND SPI   */
    if (!(devices.gpio_dev.gpio_base = (volatile u_int32_t *) ioremap(GPIO_SCORE_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"GPIO IOMEM MAPPING FAILED!");
        err = -2;
        goto err_iomap;
    }

    if (!(devices.dma_dev.dma_base = (volatile u_int32_t *) ioremap(DMA_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"DMA IOMEM MAPPING FAILED!");
        err = -2;
        goto err_iomap;
    }

    if (!(devices.spi_dev.spi_base = (volatile u_int32_t *) ioremap(SPI_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"SPI IOMEM MAPPING FAILED!");
        err = -2;
        goto err_iomap;
    }

    devices.gpio_dev.gpio_pin_spi_clk = (volatile gpio_t *)(devices.gpio_dev.gpio_base + gpio_pins[11]);
    devices.gpio_dev.gpio_pin_spi_mosi = (volatile gpio_t *)(devices.gpio_dev.gpio_base + gpio_pins[9]);
    devices.spi_dev.ssp_control_block = (volatile ssp_control_t *)devices.spi_dev.spi_base;
    devices.spi_dev.ssp_general_block = (volatile ssp_general_t *)(devices.spi_dev.spi_base + SPI_SSP_GENERAL_OFFSET);
    devices.dma_dev.dma_ch = (volatile dma_ch_t *)(devices.dma_dev.dma_base + dma_channels[devices.dma_dev.dma_ch_number]);
    devices.dma_dev.dma_cfg = (volatile dma_cfg_t *)(devices.dma_dev.dma_base + DMA_DMACCFG_OFF);

    GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__,1);
        if(devices.rgbled_type == DEV_TYPE_APA102)
    GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,1);
    return 0;

    /*  ERROR HANDLER's  */
    err_iomap:
        iounmap(devices.gpio_dev.gpio_base);
        iounmap(devices.dma_dev.dma_base);
        iounmap(devices.spi_dev.spi_base);
    err_kmalloc:
        kfree(devices.dma_dev.dma_data_ptr);
    err_set_dma_mask:
        pci_release_regions(pdev);
        pci_disable_device(pdev);
    err_request_regions:
    err_enable_device:

    return err;
}


static void pci_rgbled_remove (struct pci_dev *pdev){
    GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__,0);
        if(devices.rgbled_type == DEV_TYPE_APA102)
    GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,0);


    /*  UNMAP IO MEMORY  */
    iounmap(devices.gpio_dev.gpio_base);
    iounmap(devices.dma_dev.dma_base);
    iounmap(devices.spi_dev.spi_base);

    /*  FREE DMA MEMORY POOL  */
    kfree(devices.dma_dev.dma_data_ptr);
    
    /*  UNREGISTER PCI DEVICE  */
    pci_dev_put(pdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

}


/**** SUPPORTED DEVICES  ****/
static struct pci_device_id pci_supp_list[] = {
    { PCI_VDEVICE(INTEL, 0x0f06)},
    {0,},
};

/**** EXPORT SUPPORTED DEVS TO USER SPACE  ****/
MODULE_DEVICE_TABLE(pci, pci_supp_list);

/**** REGISTERING PCI DRIVER ****/
static struct pci_driver pdrv = {
    .name       = "rgbled_driver",
    .id_table   = pci_supp_list,
    .remove     = pci_rgbled_remove,
    .probe      = pci_rgbled_probe,
};

/**************************************************************************
***************************************************************************
***************************************************************************
*****************     INIT / CLEANUP FUNCTIONS          *******************
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


    if (alloc_chrdev_region(&first, 0, 1, "rgbled_device") < 0){
        return -1;
    }

    if ((cl = class_create(THIS_MODULE, "rgbled_device")) == NULL){
        unregister_chrdev_region(first, 1);
        return -1;
    }

    if (device_create(cl, NULL, first, NULL, "rgbled_device") == NULL){
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    cdev_init(&c_dev, &fop);

    if (cdev_add(&c_dev, first, 1) == -1){
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;

    }

    pci_register_driver(&pdrv);
    
    if(err){
        printk(KERN_ALERT"ERROR INITIALIZING RGBLED PCI DEVICE: err: %d",err);
        return -1;
    }

    if ( devices.dma_dev.dma_bar == 0){
        printk (KERN_ALERT"RGBLED driver cannot take possession of DMA device. BAR = 0x%x \n",devices.dma_dev.dma_bar);    
        printk (KERN_ALERT"Try unload/blacklist dw_dmac_pci driver before.");
        return -1;
    }else{
        *(devices.dma_dev.dma_data_ptr ) = 0xffAAffAA;
        printk (KERN_ALERT"RGBLED driver loaded BAR at 0x%x \n",devices.dma_dev.dma_bar);    
        printk (KERN_INFO"DMA data address: %p %08x \n",devices.dma_dev.dma_data_ptr, *(devices.dma_dev.dma_data_ptr));
        printk (KERN_INFO"SPI CTL BLK 1st reg: %x \n",devices.spi_dev.ssp_control_block->__sssr__);
        printk (KERN_INFO"SPI GEN BLK 1st reg: %x \n",devices.spi_dev.ssp_general_block->__prv_clock_params__);
        printk (KERN_INFO"DMA 1st reg: %x   %p\n",devices.dma_dev.dma_ch->__ctl_l__, devices.dma_dev.dma_ch);
        printk (KERN_INFO"GPIO 1st reg: %x \n",devices.gpio_dev.gpio_pin_spi_mosi->__cfg__);
    }

    

    

    return 0;

}



void __exit cleanup_module(){
    pci_unregister_driver(&pdrv);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    printk(KERN_INFO"######################### GOODBYE ##############################\n");

}
