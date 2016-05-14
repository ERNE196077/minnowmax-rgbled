

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
MODULE_DESCRIPTION("MinnowboardMAX APA102/WS281X module.");
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

#define APA102_DMADESCMEMORY(lednum)    ((sizeof(dma_channel_t) * (lednum + 2)) % 4096) > 0 ? \
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
static          __u16           dma_desc_counter;
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

        /************ SETTING GPIO ************/

        GPIO_CFG_FUNCTION(devices->gpio_pin_spi_mosi->__cfg__,1);
        if(DEV_APA102)
            GPIO_CFG_FUNCTION(devices->gpio_pin_spi_clk->__cfg__,1);

        /************ SETTING SPI ************/
        devices->ssp_control_block->__sscr0__ =
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
            SPI_SSP_SSCR0_DSS_DATASIZESELECT(0x7) ;

        devices->ssp_control_block->__sscr1__ =
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
            SPI_SSP_SSCR1_RFT_RECEIVEFIFOTRIGTHRESHOLD(0x0) |
            SPI_SSP_SSCR1_TFT_TRANSMITFIFOTRIGTHRESHOLD(0x0) |
            SPI_SSP_SSCR1_TIE_TRANSFIFOLEVELINTERRDISA |    // POSSIBLY CHANGE
            SPI_SSP_SSCR1_RIE_RECEIFIFOLEVELINTERRDISA ;    // POSSIBLY CHANGE

        //ssp_control->__ssdr__ = 0xACACACAC ;

        devices->ssp_control_block->__ssacd__ =
            SPI_SSP_SSACD_ACPS_AUDIOCLK_32_842MHZ |
            SPI_SSP_SSACD_SCDB_SYSCLKNODIVIDED |
            SPI_SSP_SSACD_ACDS_AUDIOCLKDIVIDER(2) ;

        devices->ssp_control_block->__sitf__ &= ~0xFFFF ;
        
        devices->ssp_control_block->__sitf__ |=
            SPI_SSP_SITF_SETTRANSLOWWATERMARKSPI(0x0) |
            SPI_SSP_SITF_SETTRANSHIGHWATERMARKSPI(0xFF) ;
        
        devices->ssp_general_block->__prv_clock_params__ =
            SPI_SSP_PRVCLKPARAMS_CLOCKUPDATE |
            SPI_SSP_PRVCLKPARAMS_N_DIVISOR(0xFF) |
            SPI_SSP_PRVCLKPARAMS_M_DIVIDEND(0x3) |
            SPI_SSP_PRVCLKPARAMS_ENABLECLOCK ;

        /************ SETTING DMA ************/
        dma_desc_counter = 0;
        devices->dma_cfg->__dmacfgre_l__ = 0x1;

        devices->dma_ch->__sar_l__ = 0x0;

        devices->dma_ch->__dar_l__ = SPI_BAR+0x10;

        devices->dma_ch->__ctl_l__ =
            DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGENABLE |
            DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE |
            DMA_CTL_LO_SMS_SRCMASTERLAYER1 |
            DMA_CTL_LO_DMS_DSTMASTERLAYER1 |
            DMA_CTL_LO_TTFC_FLOWCONTROLBYANYDEVICE |
            DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERDISABLE |
            DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERDISABLE |
            DMA_CTL_LO_SRCMSIZE_SRCBURSTITEMNUM(7) |
            DMA_CTL_LO_DESTMSIZE_DSTBURSTITEMNUM(7) |
            DMA_CTL_LO_SINC_SOURCEADDRNOCHANGE |
            DMA_CTL_LO_DINC_DESTADDRNOCHANGE |
            DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF32BITS |
            DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF32BITS |
            DMA_CTL_LO_INTEN_INTERRUPTSENABLED ;
    
        devices->dma_ch->__llp_l__ = DMA_LLP_LO_ADDRESSOFNEXTLLP((intptr_t)addr2);
        
        devices->dma_ch->__rsv_0x14__ = (intptr_t)(addr2 >> 29);

        devices->dma_ch->__ctl_h__ =
            DMA_CTL_HI_DONE_DONEBITZERO |
            DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(1) ;

        devices->dma_ch->__cfg_l__ =
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
            DMA_CFG_LO_FIFOEMPTY_SETVALUEZERO |
            DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
            DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY ;

        devices->dma_ch->__cfg_h__ =
            DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(0x0) |
            DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
            DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
            DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
            DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
            DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

        devices->dma_ch->__sgr_l__ = DMA_SGR_LO_SGI_SRCGATHERINCDECMULTIPLE(64);

        devices->dma_ch->__dsr_l__ = DMA_DSR_LO_DSI_DESTSCATTERINCDECMULTIPLE(32);

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



  if (alloc_chrdev_region(&first, 0, 1, "minnowleddev") < 0)
  {
    return -1;
  }
    if ((cl = class_create(THIS_MODULE, "minnowleddev")) == NULL)
  {
    unregister_chrdev_region(first, 1);
    return -1;
  }
    if (device_create(cl, NULL, first, NULL, "minnowleddev") == NULL)
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
printk(KERN_INFO"######################### GOODBYE ##############################\n");

}
