    

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
#include <linux/interrupt.h>
#include <linux/sched.h>
//#include <linux/tqueue.h>

#include "rgbled_driver.h"
#include "headers/rgbled_devices.h"



MODULE_LICENSE("GPL");
MODULE_AUTHOR("ernestrc");
MODULE_DESCRIPTION("MinnowboardMAX APA102/WS281X module.");
MODULE_SUPPORTED_DEVICE("MinnowboardMAX");
MODULE_INFO(intree, "Y");

/********* DEVICE VARIABLES  ************/
static int open = 0 ;
static dev_t first;         // Global variable for the first device number
static struct cdev c_dev;   // Global variable for the character device structure
static struct class *cl;    // Global variable for the device class

int k ;

/********* PCI VARIABLES  ************/
static int err_dma, err_spi, sscr1, ssdr, sscr0, count =0;

/********* RGBLED VARIABLES  ************/

devices_t    devices;
__u32 i, count3 = 0 ;   
__u32 test_num = 0;
__u32 result;
__u32 spi_irq_sta = 0;
__u32 fifo_level = 0;
__u32 rx_fifo_level = 0;
__u32 *ptrDmaRxData;
__u32 dma_combined_int = 0;
__u32 dma_clrreg;
volatile dma_cfg_t *dma_gen_regs ;
static dma_item_t *dma_list;


/********* SSP CONFIG VARIABLES  ************/
__u32 ssp_sscr0_cfg = 
            SPI_SSP_SSCR0_MOD_NORMALSSPMODE |
            SPI_SSP_SSCR0_ACS_CLOCKBYAUDIOCLOCK |
            SPI_SSP_SSCR0_TIM_NOINTERRUPTFIFOUNDERRUN |
            SPI_SSP_SSCR0_RIM_INTERRUPTONFIFOOVERRUN |
            SPI_SSP_SSCR0_NCS_CLOCKBYECS |
            SPI_SSP_SSCR0_EDSS_ONEPREPENDTODSS |
            SPI_SSP_SSCR0_SCR_SERIALCLOCKRATE(20) |
            SPI_SSP_SSCR0_SSE_SSPDISABLE |
            SPI_SSP_SSCR0_ECS_EXTERNALCLOCKFROMGPIO |
            SPI_SSP_SSCR0_FRF_MOTOROLA |
            SPI_SSP_SSCR0_DSS_DATASIZESELECT(0x7) ;

__u32 ssp_sscr1_cfg = 
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
            SPI_SSP_SSCR1_RFT_RECEIVEFIFOTRIGTHRESHOLD(0x9) |
            SPI_SSP_SSCR1_TFT_TRANSMITFIFOTRIGTHRESHOLD(0xC) |
            SPI_SSP_SSCR1_TIE_TRANSFIFOLEVELINTERRENA |    // POSSIBLY CHANGE
            SPI_SSP_SSCR1_RIE_RECEIFIFOLEVELINTERRDISA ; 
__u32 ssp_ssacd_cfg = 
            SPI_SSP_SSACD_ACPS_AUDIOCLK_32_842MHZ |
            SPI_SSP_SSACD_SCDB_SYSCLKNODIVIDED |
            SPI_SSP_SSACD_ACDS_AUDIOCLKDIVIDER(2) ;

__u32 ssp_sssr_clearint_mask = ~(SPI_SSP_SSSR_BCE_BITCOUNTERROR |
            SPI_SSP_SSSR_TUR_TRANSMITFIFOUNDERRUN |
            SPI_SSP_SSSR_CSS_CLOCKSYNCSTATUS |
            SPI_SSP_SSSR_EOC_ENDOFDMACHAIN |
            SPI_SSP_SSSR_TINT_RECEIVERTIMEOUTINTERRUPT |
            SPI_SSP_SSSR_PINT_PENDINGTRAILINGBYTE | 
            SPI_SSP_SSSR_ROR_RECEIVEFIFOOVERRUN |
            SPI_SSP_SSSR_RFS_RECEIVEFIFOSERVICEREQ |
            SPI_SSP_SSSR_TFS_TRANSMITFIFOSERVICEREQ );

/********* DMA TX CONFIG VARIABLES  ************/
__u32 dma_tx_ctl_l_cfg =
            DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGDISABLE |
            DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE |
            DMA_CTL_LO_SMS_SRCMASTERLAYER1 |
            DMA_CTL_LO_DMS_DSTMASTERLAYER1 |
            DMA_CTL_LO_TTFC_FLOWCONTROLBYDMA |
            //DMA_CTL_LO_TTFC_FLOWCONTROLBYDEST |
            DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERDISABLE |
            DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERDISABLE |
            DMA_CTL_LO_SRCMSIZE_SRCBURSTITEMNUM(0x7) |
            DMA_CTL_LO_DESTMSIZE_DSTBURSTITEMNUM(0x7) |
            DMA_CTL_LO_SINC_SOURCEADDRINCREMENT |
            DMA_CTL_LO_DINC_DESTADDRNOCHANGE |
            DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF32BITS |
            DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF32BITS |
            DMA_CTL_LO_INTEN_INTERRUPTSENABLED ;
__u32 dma_tx_ctl_h_cfg = 
            DMA_CTL_HI_DONE_DONEBITZERO |
            DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0x40) ; // Size of SSP fifo
__u32 dma_tx_cfg_l_cfg = 
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
            DMA_CFG_LO_HSSELDST_DESTHWHANDSHAKING |
            DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
            DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY ;
__u32 dma_tx_cfg_h_cfg = 
            DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(19) |
            DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
            DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
            DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
            DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
            DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

/********* DMA RX CONFIG VARIABLES  ************/
__u32 dma_rx_ctl_l_cfg =
            DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGDISABLE |
            DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE |
            DMA_CTL_LO_SMS_SRCMASTERLAYER1 |
            DMA_CTL_LO_DMS_DSTMASTERLAYER1 |
            DMA_CTL_LO_TTFC_FLOWCONTROLBYDMA |
            //DMA_CTL_LO_TTFC_FLOWCONTROLBYDEST |
            DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERDISABLE |
            DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERDISABLE |
            DMA_CTL_LO_SRCMSIZE_SRCBURSTITEMNUM(0x7) |
            DMA_CTL_LO_DESTMSIZE_DSTBURSTITEMNUM(0x7) |
            DMA_CTL_LO_SINC_SOURCEADDRNOCHANGE |
            DMA_CTL_LO_DINC_DESTADDRNOCHANGE |
            DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF32BITS |
            DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF32BITS |
            DMA_CTL_LO_INTEN_INTERRUPTSDISABLED ;
__u32 dma_rx_ctl_h_cfg = 
            DMA_CTL_HI_DONE_DONEBITZERO |
            DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0xFF0) ; // Size of SSP fifo
__u32 dma_rx_cfg_l_cfg = 
            DMA_CFG_LO_RELOADDST_NORELOADDSTAFTERTRANSFER |
            DMA_CFG_LO_RELOADSRC_NORELOADSRCAFTERTRANSFER |
            DMA_CFG_LO_MAXABRST_NOLIMITBURSTTRANSFER |
            DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVEHIGH |
            DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVEHIGH |
            DMA_CFG_LO_LOCKB_BUSNOTLOCKED |
            DMA_CFG_LO_LOCKCH_CHANNELNOTLOCKED |
            DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSFER |
            DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSFER |
            DMA_CFG_LO_HSSELSRC_SOURCEHWHANDSHAKING |               // TO BE CHANGED PROBABLY
            DMA_CFG_LO_HSSELDST_DESTHWHANDSHAKING |
            DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
            DMA_CFG_LO_CHPRIOR_LOWESTPRIORITY ;
__u32 dma_rx_cfg_h_cfg = 
            DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(0x19) |
            DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
            DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
            DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
            DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
            DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

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
*****************     INTERRUPT HANDLERS          *************************
***************************************************************************
***************************************************************************
***************************************************************************/


static irqreturn_t short_spi_interrupt(int irq, void *dev_id)
{
    spi_dev_t *spi_dev = (spi_dev_t *)dev_id;
    __u32 ssdr, sssr;
    
    fifo_level = (spi_dev->ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;
    spi_irq_sta = spi_dev->ssp_control_block->__sssr__;
    sscr1 = devices.spi_dev.ssp_control_block->__sscr1__;
    
    printk(KERN_INFO"***** SPI interrupt :: STATUS : 0x%08x :: FIFO level : %d :: SSCR1 : %08x\n",spi_irq_sta, fifo_level,sscr1);
           
    /* If the TX FIFO is below the limit set, TFS flag will activate. 
    This flag can only be cleared until the FIFO is empty. Due this limitant 
    is needed to poll the FIFO level manually and request a DMA transfer, which 
    should be ready from the DMA interrupt callback */
    if ( spi_dev->ssp_control_block->__sssr__ & SPI_SSP_SSSR_TFS_TRANSMITFIFOSERVICEREQ ){
        

        if ( fifo_level < 100){
            devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number); // | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));
            
            count++;

            if( count > 0){
                do {
                     while ( spi_dev->ssp_control_block->__sssr__ & SPI_SSP_SSSR_RNE_RECEIVEFIFONOTEMPTY) {
                             ssdr = spi_dev->ssp_control_block->__ssdr__;
                     }
                } while ( spi_dev->ssp_control_block->__sssr__ & SPI_SSP_SSSR_BSY_SPIBUSY );    
                spi_dev->ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;
            }   
        }
        
        fifo_level = (spi_dev->ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;
        spi_irq_sta = spi_dev->ssp_control_block->__sssr__;
        sscr1 = devices.spi_dev.ssp_control_block->__sscr1__;
        
        printk(KERN_INFO"***** SPI interrupt Inside Handler :: STATUS : 0x%08x :: FIFO level : %d :: SSCR1 : %08x\n",spi_irq_sta, fifo_level,sscr1);
        

        
        //sscr1 &= ( SPI_SSP_SSCR1_TFT_TRANSMITFIFOTRIGTHRESHOLD(8) | SPI_SSP_SSCR1_RFT_RECEIVEFIFOTRIGTHRESHOLD(8) );

        //spi_dev->ssp_control_block->__sscr1__ = sscr1 ;
        
        
         
        
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}


    /**/
    
static irqreturn_t short_dma_interrupt(int irq, void *dev_id)
{
    dma_dev_t *dma_dev = (dma_dev_t *)dev_id;
    __u32 status = dma_dev->dma_cfg->__statusint__;

    
    fifo_level = (devices.spi_dev.ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;
        spi_irq_sta = devices.spi_dev.ssp_control_block->__sssr__;
        sscr1 = devices.spi_dev.ssp_control_block->__sscr1__;
        
        printk(KERN_INFO"##### DMA INTERRUPT SPI info:: STATUS : 0x%08x :: FIFO level : %d :: SSCR1 : %08x\n",spi_irq_sta, fifo_level,sscr1);
        
    // Confirm the interruption is provoked by the DMA 
    if ( status & 0x1F ){
        /*
        fifo_level = (devices.spi_dev.ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;
        spi_irq_sta = devices.spi_dev.ssp_control_block->__sssr__;
        sscr1 = devices.spi_dev.ssp_control_block->__sscr1__;
        
        printk(KERN_INFO"******************************\n");
        printk(KERN_INFO"Combined Status 0x%08x \n",status);
        printk(KERN_INFO"Raw Status Tfr 0x%08x \n",dma_gen_regs->__rawtfr__);
        printk(KERN_INFO"Raw Status Block 0x%08x \n",dma_gen_regs->__rawblock__);
        printk(KERN_INFO"Raw Status Src Tran 0x%08x \n",dma_gen_regs->__rawsrctran__);
        printk(KERN_INFO"Raw Status Dest Tran 0x%08x \n",dma_gen_regs->__rawdsttran__);
        printk(KERN_INFO"Raw Status Err 0x%08x \n",dma_gen_regs->__rawerr__);
        printk(KERN_INFO"Status Tfr 0x%08x \n",dma_gen_regs->__statustfr__);
        printk(KERN_INFO"Status Block 0x%08x \n",dma_gen_regs->__statusblock__);
        printk(KERN_INFO"Status Src Tran 0x%08x \n",dma_gen_regs->__statussrctran__);
        printk(KERN_INFO"Status Dest Tran 0x%08x \n",dma_gen_regs->__statusdsttran__);
        printk(KERN_INFO"Status Err 0x%08x \n",dma_gen_regs->__statuserr__);
        printk(KERN_INFO"***************************");
        */
        //printk(KERN_INFO"FROM : DMA INTERRUPT :\nSPI info \nSTATUS : 0x%08x\nFIFO level : %d\n SSCR1 : %08x\nCount3 : %d\n DMA_SRCADDR : %08x\n\n",spi_irq_sta, fifo_level,sscr1,count3,devices.dma_dev.dma_tx_ch->__sar_l__);
        
        // Stop DMA channel
        //devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        printk(KERN_INFO"##### DMA INTERRUPT Inside Handler :: Raw Status Tfr 0x%08x \n",dma_dev->dma_cfg->__rawtfr__);
        if ( dma_dev->dma_cfg->__rawtfr__ & DMA_INT_STATUSREG_MASK(1)){
            devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1)));

            /************ SETTING RX DMA ************/
            devices.dma_dev.dma_rx_ch->__sar_l__ = SPI_BASE_ADDR + 0x10;
            devices.dma_dev.dma_rx_ch->__dar_l__ = (__u32)ptrDmaRxData;
            devices.dma_dev.dma_rx_ch->__llp_l__ = 0x0;
            devices.dma_dev.dma_rx_ch->__ctl_l__ = dma_rx_ctl_l_cfg;
            devices.dma_dev.dma_rx_ch->__ctl_h__ = dma_rx_ctl_h_cfg;
            devices.dma_dev.dma_rx_ch->__cfg_l__ = dma_rx_cfg_l_cfg;
            devices.dma_dev.dma_rx_ch->__cfg_h__ = dma_rx_cfg_h_cfg;
            devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));
        }
        // Clear DMA Interrupts
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__cleartfr__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__clearblock__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__clearsrctran__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__cleardsttran__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__clearerr__ ));
        // Refresh DMA Interrupt Masks - Only activate transmission complete interrupt
        __raw_writel(0xFFFF , &(dma_dev->dma_cfg->__masktfr__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__maskblock__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__masksrctran__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__maskdsttran__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__maskerr__ ));

        // Reconfigure DMA Channel
        dma_dev->dma_tx_ch->__sar_l__ = devices.dma_dev.dma_data_phys;
        dma_dev->dma_tx_ch->__dar_l__ = SPI_BASE_ADDR + 0x10;
        dma_dev->dma_tx_ch->__llp_l__ = 0x0;
        dma_dev->dma_tx_ch->__ctl_l__ = dma_tx_ctl_l_cfg;
        dma_dev->dma_tx_ch->__ctl_h__ = dma_tx_ctl_h_cfg;
        dma_dev->dma_tx_ch->__cfg_l__ = dma_tx_cfg_l_cfg;
        dma_dev->dma_tx_ch->__cfg_h__ = dma_tx_cfg_h_cfg;


        
        /*printk(KERN_INFO"*********************************\n");
        printk(KERN_INFO"__src_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__sar_l__ );
        printk(KERN_INFO"__dst_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__dar_l__);
        printk(KERN_INFO"__llp_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__llp_l__);
        printk(KERN_INFO"__ctl_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__ctl_l__);
        printk(KERN_INFO"__ctl_h__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__ctl_h__);
        printk(KERN_INFO"__cfg_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__cfg_l__);
        printk(KERN_INFO"__cfg_h__\t\t%08x\n\n", devices.dma_dev.dma_tx_ch->__cfg_h__);
        */
        return IRQ_HANDLED;
    }
    
    return IRQ_NONE;    
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

    /**************************************************************************
    *****************     IOCTL - SET RGBLED CONFIG          ******************
    ***************************************************************************/
    case IOCTL_RGBLED_SETCONFIG:
        get_user(devices.rgbled_config, (__u32 *)ioctl_params );

        /* Print configuration information */
        printk(KERN_ALERT"Type: %d  LedNum: %d  DMACh: %d  Anim: %d\n\n",RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config) >> 28, 
                                                                RGBLED_CONF_GET_LEDNUM(devices.rgbled_config), 
                                                                RGBLED_CONF_GET_DMACH(devices.rgbled_config) >> 24 ,
                                                                RGBLED_CONF_GET_ANIMATION(devices.rgbled_config) >> 16);
        
        /* Set DMA channel to use */
        devices.dma_dev.dma_ch_number = RGBLED_CONF_GET_DMACH(devices.rgbled_config);
        
        /* Set DMA total transmission size - In Bytes */
        devices.dma_dev.dma_data_size = (RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config)) ? 
                                        RGBLED_DATA_SIZE_APA102 (RGBLED_CONF_GET_LEDNUM(devices.rgbled_config)): 
                                        RGBLED_DATA_SIZE_WS281X (RGBLED_CONF_GET_LEDNUM(devices.rgbled_config));
        
        /*  Allocate the PCI memory pool for the DMA data  */
        devices.dma_dev.dma_pool = pci_pool_create( "rgbled_dma_pool", pdev, RGBLED_DATA_DMA_POOL_SIZE(devices.dma_dev.dma_data_size), 32, 0 );
        devices.dma_dev.dma_data_ptr = pci_pool_alloc( devices.dma_dev.dma_pool, GFP_ATOMIC, &devices.dma_dev.dma_data_phys); 
        if (devices.dma_dev.dma_data_ptr == NULL){
            printk(KERN_ALERT"Cannot allocate pci memory block for DMA data, please release memory and try again");
        }

        /* "memset" DMA data memory */
        for(i = 0 ; i < devices.dma_dev.dma_data_size ; i++)
            *((__u8 *)devices.dma_dev.dma_data_ptr + i) = 0x00;    
        
        /* Set GPIO pins */
        /* If SPI MOSI is already configured, leave it */
        if(!(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__ & 0x1))
            GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__,1);
        /* Configure SPI CLK if APA102 LEDs are used */
        if(RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config))
            if(!(devices.gpio_dev.gpio_pin_spi_clk->__cfg__ & 0x1))    
                GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,1);
    

        /* Map DMA channel registers to a device pointer */
        devices.dma_dev.dma_tx_ch = (volatile dma_ch_t *)(devices.dma_dev.dma_base + dma_channels[devices.dma_dev.dma_ch_number]);
        devices.dma_dev.dma_rx_ch = (volatile dma_ch_t *)(devices.dma_dev.dma_base + dma_channels[devices.dma_dev.dma_ch_number+1]);

        pci_pool_free(devices.dma_dev.dma_pool, devices.dma_dev.dma_data_ptr, devices.dma_dev.dma_data_phys);
        pci_pool_destroy(devices.dma_dev.dma_pool);
    

        break;


    /**************************************************************************
    *****************     IOCTL - SET RGBLED CONFIG          ******************
    ***************************************************************************/
    case IOCTL_RGBLED_CONFIGURE:

        /************ SETTING DMA MEMORY ************/
        dma_gen_regs = devices.dma_dev.dma_cfg;
        
        

        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1)));
        
        if (devices.dma_dev.dma_irqn >= 0) {
            result = request_irq(devices.dma_dev.dma_irqn, short_dma_interrupt,
                                 IRQF_SHARED, "rgbled_dma_driver", &(devices.dma_dev));
            if (result) {
                printk(KERN_INFO "DMA: can't get assigned irq %i\n", devices.dma_dev.dma_irqn);
                devices.dma_dev.dma_irqn = -1;
            }
            else {
            // actually enable it -- assume this *is* a parallel port 
                 printk(KERN_INFO"DMA interrupt REGISTERED!!\n");
            }
        }

        if (devices.spi_dev.spi_irqn >= 0) {
            result = request_irq(devices.spi_dev.spi_irqn, short_spi_interrupt, 
                                 IRQF_SHARED, "rgbled_spi_driver", &(devices.spi_dev));
            if (result) {
                printk(KERN_INFO "SPI: can't get assigned irq %i\n", devices.spi_dev.spi_irqn);
                devices.spi_dev.spi_irqn = -1;
            }
            else { 
                 printk(KERN_INFO"SPI interrupt REGISTERED!!\n");
            }
        }
        /*  CREATE DMA MEMORY POOL FOR THE RX DMA DATA  */
        ptrDmaRxData = kmalloc(sizeof(__u32), GFP_DMA);
        
        /* Initialize DMA data to be transmitted */
        rgbled_test(devices.dma_dev.dma_data_ptr, test_num);
        printk(KERN_INFO"DMA RX Data phys %p %08x \n", ptrDmaRxData, (__u32)ptrDmaRxData);

        
        
        /************ SETTING SPI ************/
        devices.spi_dev.ssp_control_block->__sscr0__ = ssp_sscr0_cfg; 
        devices.spi_dev.ssp_control_block->__sscr1__ = ssp_sscr1_cfg; 
        devices.spi_dev.ssp_control_block->__sssr__ &= 
            ~(SPI_SSP_SSSR_BCE_BITCOUNTERROR |
            SPI_SSP_SSSR_TUR_TRANSMITFIFOUNDERRUN |
            SPI_SSP_SSSR_CSS_CLOCKSYNCSTATUS |
            SPI_SSP_SSSR_EOC_ENDOFDMACHAIN |
            SPI_SSP_SSSR_TINT_RECEIVERTIMEOUTINTERRUPT |
            SPI_SSP_SSSR_PINT_PENDINGTRAILINGBYTE | 
            SPI_SSP_SSSR_ROR_RECEIVEFIFOOVERRUN |
            SPI_SSP_SSSR_RFS_RECEIVEFIFOSERVICEREQ |
            SPI_SSP_SSSR_TFS_TRANSMITFIFOSERVICEREQ );
        devices.spi_dev.ssp_control_block->__ssacd__ = ssp_ssacd_cfg;
        devices.spi_dev.ssp_control_block->__sitf__ &= ~0xFFFF ;
        devices.spi_dev.ssp_control_block->__sitf__ |=
            SPI_SSP_SITF_LWMTF_SETTXLOWWATERMARKSPI(0xFF)    |
            SPI_SSP_SITF_HWMTF_SETTXHIGHWATERMARKSPI(0xFF) ;
            
       devices.spi_dev.ssp_general_block->__prv_clock_params__ =
            SPI_SSP_PRVCLKPARAMS_CLOCKUPDATE |
            SPI_SSP_PRVCLKPARAMS_N_DIVISOR(0xFF) |
            SPI_SSP_PRVCLKPARAMS_M_DIVIDEND(0x3) |
            SPI_SSP_PRVCLKPARAMS_ENABLECLOCK ;



        /************ SETTING TX DMA ************/
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = 0x1;
        devices.dma_dev.dma_tx_ch->__sar_l__ = devices.dma_dev.dma_data_phys;
        devices.dma_dev.dma_tx_ch->__dar_l__ = SPI_BASE_ADDR + 0x10;
        devices.dma_dev.dma_tx_ch->__llp_l__ = 0x0;
        devices.dma_dev.dma_tx_ch->__ctl_l__ = dma_tx_ctl_l_cfg;
        devices.dma_dev.dma_tx_ch->__ctl_h__ = dma_tx_ctl_h_cfg;
        devices.dma_dev.dma_tx_ch->__cfg_l__ = dma_tx_cfg_l_cfg;
        devices.dma_dev.dma_tx_ch->__cfg_h__ = dma_tx_cfg_h_cfg;
        
        /************ SETTING RX DMA ************/
        devices.dma_dev.dma_rx_ch->__sar_l__ = SPI_BASE_ADDR + 0x10;
        devices.dma_dev.dma_rx_ch->__dar_l__ = (__u32)ptrDmaRxData;
        devices.dma_dev.dma_rx_ch->__llp_l__ = 0x0;
        devices.dma_dev.dma_rx_ch->__ctl_l__ = dma_rx_ctl_l_cfg;
        devices.dma_dev.dma_rx_ch->__ctl_h__ = dma_rx_ctl_h_cfg;
        devices.dma_dev.dma_rx_ch->__cfg_l__ = dma_rx_cfg_l_cfg;
        devices.dma_dev.dma_rx_ch->__cfg_h__ = dma_rx_cfg_h_cfg;

        /************ CLEAR INTERRUPTS AND ADD DMA MASK INTERRUPTS ************/ 
        __raw_writel(0xFF , &(dma_gen_regs->__cleartfr__));
        __raw_writel(0xFF , &(dma_gen_regs->__clearblock__));
        __raw_writel(0xFF , &(dma_gen_regs->__clearsrctran__));
        __raw_writel(0xFF , &(dma_gen_regs->__cleardsttran__));
        __raw_writel(0xFF , &(dma_gen_regs->__clearerr__ ));
        
        __raw_writel(0xFFFF , &(dma_gen_regs->__masktfr__));
        __raw_writel(0xFF00 , &(dma_gen_regs->__maskblock__));
        __raw_writel(0xFF00 , &(dma_gen_regs->__masksrctran__));
        __raw_writel(0xFF00 , &(dma_gen_regs->__maskdsttran__));
        __raw_writel(0xFF00 , &(dma_gen_regs->__maskerr__ ));
        
        /************ ENABLE DMA CONTROLLER ************/
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = DMA_DMACFGREG_L_DMA_ENA;

        break;

    case IOCTL_RGBLED_DECONFIGURE:

        /* Disable DMA Channel */
        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        /* Disable DMA Request for the Chnannel */
        devices.dma_dev.dma_cfg->__reqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        devices.dma_dev.dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        /* Disable DMA Single Request for the Chnannel */
        devices.dma_dev.dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        devices.dma_dev.dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        /* Disable SSP */
        devices.spi_dev.ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;

        /* Configure SPI MOSI pin */
        GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__,0);
        
        /* If LED is APA102 class, configure SPI CLK pin */
        if(RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config))
            GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,0);

        break;

    case IOCTL_RGBLED_RENDER:

        test_num++;
        rgbled_test(devices.dma_dev.dma_data_ptr, test_num);
        
        __clear_cache((char *)devices.dma_dev.dma_data_ptr,
                        (char *)(devices.dma_dev.dma_data_ptr + 5 ));

        *ptrDmaRxData = 0;

        /* Enable SSP */
        

        /* PRINT USEFUL INFORMATION */
        spi_irq_sta = devices.spi_dev.ssp_control_block->__sssr__;
        sscr0 = devices.spi_dev.ssp_control_block->__sscr0__;
        sscr1 = devices.spi_dev.ssp_control_block->__sscr1__;
        fifo_level = (devices.spi_dev.ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;
        rx_fifo_level = (devices.spi_dev.ssp_control_block->__sirf__ & SPI_SSP_SIRF_SIRFL_READRXFIFOENTRIESSPI) >> 8 ;
        printk(KERN_ALERT"SPI  \nSTATUS : 0x%08x\nTX FIFO level : %d\nRX FIFO level : %d\nSSCR0 : %08x\n SSCR1 : %08x\nDMA SRCADDR : %08x\nPTR RX DATA : %d\n\n",spi_irq_sta, fifo_level,rx_fifo_level,sscr0, sscr1,devices.dma_dev.dma_tx_ch->__sar_l__,*ptrDmaRxData);


        
        while ( devices.spi_dev.ssp_control_block->__sssr__ & SPI_SSP_SSSR_RNE_RECEIVEFIFONOTEMPTY) {
            ssdr = devices.spi_dev.ssp_control_block->__ssdr__;
        }
        /* Enable DMA Channel */
        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));
        //udelay(70);
        devices.spi_dev.ssp_control_block->__sscr0__ |= SPI_SSP_SSCR0_SSE_SSPENABLE;    
        
        //devices.dma_dev.dma_cfg->__reqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));
        //devices.dma_dev.dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));
        
        /* PRINT USEFUL INFORMATION */
        //udelay(45);
        /*
        spi_irq_sta = devices.spi_dev.ssp_control_block->__sssr__;
        sscr0 = devices.spi_dev.ssp_control_block->__sscr0__;
        sscr1 = devices.spi_dev.ssp_control_block->__sscr1__;
        fifo_level = (devices.spi_dev.ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;
        rx_fifo_level = (devices.spi_dev.ssp_control_block->__sirf__ & SPI_SSP_SIRF_SIRFL_READRXFIFOENTRIESSPI) >> 8 ;
        printk(KERN_ALERT"SPI  \nSTATUS : 0x%08x\nTX FIFO level : %d\nRX FIFO level : %d\nSSCR0 : %08x\n SSCR1 : %08x\nDMA SRCADDR : %08x\nPTR RX DATA : %d\n\n",spi_irq_sta, fifo_level,rx_fifo_level,sscr0, sscr1,devices.dma_dev.dma_tx_ch->__sar_l__,*ptrDmaRxData);
        */
        //devices.spi_dev.ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE ;


        break;
    
    
    case IOCTL_DMA_PRINTITEMS:
        printk(KERN_ALERT"DMA data address: %p\n\n",devices.dma_dev.dma_data_ptr);
        printk(KERN_ALERT"DMA mem size : %08x\n\n",devices.dma_dev.dma_data_size);
        printk(KERN_INFO"__chenreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__chenreg_l__ );
        printk(KERN_INFO"__dmacfgre_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__dmacfgre_l__ );
        printk(KERN_INFO"__reqdstreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__reqdstreg_l__);
        printk(KERN_INFO"__reqsrcreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__reqsrcreg_l__);
        printk(KERN_INFO"__sglrqdstreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__sglrqdstreg_l__);
        printk(KERN_INFO"__sglrqsrcreg_l__\t\t%08x\n\n", devices.dma_dev.dma_cfg->__sglrqsrcreg_l__);
    
        printk(KERN_INFO"__src_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__sar_l__ );
        printk(KERN_INFO"__dst_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__dar_l__);
        printk(KERN_INFO"__llp_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__llp_l__);
        printk(KERN_INFO"__ctl_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__ctl_l__);
        printk(KERN_INFO"__ctl_h__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__ctl_h__);
        printk(KERN_INFO"__cfg_l__\t\t%08x\n", devices.dma_dev.dma_tx_ch->__cfg_l__);
        printk(KERN_INFO"__cfg_h__\t\t%08x\n\n", devices.dma_dev.dma_tx_ch->__cfg_h__);

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
static int pci_rgbled_spi_probe (struct pci_dev *pdev, const struct pci_device_id *id){

    /*  REGISTER PCI DEVICE  */
    err_spi = pci_enable_device(pdev);
    if (err_spi)
        goto err_enable_spi_device;

    err_spi = pci_request_regions(pdev, "rgbled_spi_driver");
    if (err_spi)
        goto err_request_spi_regions;

    
    devices.pdev_spi = pci_dev_get(pdev);
    devices.spi_dev.spi_bar = pci_resource_start (pdev, 0);         /*  GET SPI BASE ADDRESS REGISTER  */
    devices.spi_dev.spi_bar_size = pci_resource_len (pdev, 0);      /*  GET SPI BASE ADDRESS REGISTER SIZE  */
    pci_set_master(pdev);                           /*  ENABLE DEVICE AS MASTER  */

    
    /*  MAP IO MEM FOR GPIO, DMA AND SPI   */
    if (!(devices.gpio_dev.gpio_base = (volatile uint32_t *) ioremap(GPIO_SCORE_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"GPIO IOMEM MAPPING FAILED!");
        err_spi = -2;
        goto err_spi_iomap;
    }

    if (!(devices.spi_dev.spi_base = (volatile uint32_t *) ioremap(SPI_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"SPI IOMEM MAPPING FAILED!");
        err_spi = -2;
        goto err_spi_iomap;
    }

    devices.spi_dev.spi_irqn = SPI_IRQn;
    devices.gpio_dev.gpio_pin_spi_clk = (volatile gpio_t *)(devices.gpio_dev.gpio_base + gpio_pins[11]);
    devices.gpio_dev.gpio_pin_spi_mosi = (volatile gpio_t *)(devices.gpio_dev.gpio_base + gpio_pins[9]);
    devices.spi_dev.ssp_control_block = (volatile ssp_control_t *)devices.spi_dev.spi_base;
    devices.spi_dev.ssp_general_block = (volatile ssp_general_t *)(devices.spi_dev.spi_base + SPI_SSP_GENERAL_OFFSET);
    /*
    if (devices.spi_dev.spi_irqn >= 0) {
        
        result = request_irq(devices.spi_dev.spi_irqn, short_spi_interrupt, 
                             IRQF_SHARED, "rgbled_spi_driver", &(devices.spi_dev));
        if (result) {
            printk(KERN_INFO "SPI: can't get assigned irq %i\n", devices.spi_dev.spi_irqn);
            devices.spi_dev.spi_irqn = -1;
        }
        else { 
             printk(KERN_INFO"SPI interrupt REGISTERED!!\n");
        }
    }
    */
    return 0;

    /*  ERROR HANDLER's  */
    err_spi_iomap:
        iounmap(devices.gpio_dev.gpio_base);
        iounmap(devices.spi_dev.spi_base);
    
    err_request_spi_regions:
    err_enable_spi_device:

    return err_spi;
}

static int pci_rgbled_dma_probe (struct pci_dev *pdev, const struct pci_device_id *id){

    /*  REGISTER PCI DEVICE  */
    err_dma = pci_enable_device(pdev);
    if (err_dma)
        goto err_enable_dma_device;

    err_dma = pci_request_regions(pdev, "rgbled_dma_driver");
    if (err_dma)
        goto err_request_dma_regions;

    err_dma = pci_set_dma_mask(pdev, DMA_BIT_MASK(29));
    if (err_dma)
        goto err_set_dma_mask;

    err_dma = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(29));
    if (err_dma)
        goto err_set_dma_mask;

    /*  GET DMA BASE ADDRESS REGISTER  */   
    devices.pdev_dma = pci_dev_get(pdev);
    devices.dma_dev.dma_bar = pci_resource_start (pdev, 0);  
    /*  GET DMA BASE ADDRESS REGISTER SIZE  */       
    devices.dma_dev.dma_bar_size = pci_resource_len (pdev, 0);   
    /*  ENABLE DEVICE AS DMA  */   
    pci_set_master(pdev);                          

    

    if (!(devices.dma_dev.dma_base = (volatile uint32_t *) ioremap(DMA_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"DMA IOMEM MAPPING FAILED!");
        err_dma = -2;
        goto err_dma_iomap;
    }

    /* Set DMA controller interrupt id */
    devices.dma_dev.dma_irqn = DMA_IRQn;

    /* Map DMA general registers to a device pointer */
    devices.dma_dev.dma_cfg = (volatile dma_cfg_t *)(devices.dma_dev.dma_base + DMA_DMACCFG_OFF);
    


    return 0;

    /*  ERROR HANDLER's  */
    err_dma_iomap:
        iounmap(devices.dma_dev.dma_base);
    err_set_dma_mask:
        pci_release_regions(pdev);
        pci_disable_device(pdev);
    err_request_dma_regions:
    err_enable_dma_device:

    return err_dma;
}


static void pci_rgbled_spi_remove (struct pci_dev *pdev){
    
    /*  UNMAP IO MEMORY  */
    free_irq(devices.spi_dev.spi_irqn, &(devices.spi_dev));
    
    iounmap(devices.gpio_dev.gpio_base);
    iounmap(devices.spi_dev.spi_base);

    /*  UNREGISTER PCI DEVICE  */
    pci_dev_put(pdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

}

static void pci_rgbled_dma_remove (struct pci_dev *pdev){
    free_irq(devices.dma_dev.dma_irqn, &(devices.dma_dev));

    /*  UNMAP IO MEMORY  */
    iounmap(devices.dma_dev.dma_base);
    
       /*  FREE DMA MEMORY POOL  */
    pci_pool_free(devices.dma_dev.dma_pool, devices.dma_dev.dma_data_ptr, devices.dma_dev.dma_data_phys);
    pci_pool_destroy(devices.dma_dev.dma_pool);
 
    /*  UNREGISTER PCI DEVICE  */
    pci_dev_put(pdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

}


/**** SUPPORTED DEVICES  ****/
static struct pci_device_id pci_supp_dma_list[] = {
    { PCI_VDEVICE(INTEL, 0x0f06)},
    {0,},
};

static struct pci_device_id pci_supp_spi_list[] = {
    { PCI_VDEVICE(INTEL, 0x0f0e)},
    {0,},
};
/**** EXPORT SUPPORTED DEVS TO USER SPACE  ****/
MODULE_DEVICE_TABLE(pci, pci_supp_dma_list);
MODULE_DEVICE_TABLE(pci, pci_supp_spi_list);

/**** REGISTERING PCI DRIVER ****/
static struct pci_driver pdrv_dma = {
    .name       = "rgbled_dma_driver",
    .id_table   = pci_supp_dma_list,
    .remove     = pci_rgbled_dma_remove,
    .probe      = pci_rgbled_dma_probe,
};

static struct pci_driver pdrv_spi = {
    .name       = "rgbled_spi_driver",
    .id_table   = pci_supp_spi_list,
    .remove     = pci_rgbled_spi_remove,
    .probe      = pci_rgbled_spi_probe,
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

    pci_register_driver(&pdrv_dma);
    pci_register_driver(&pdrv_spi);
    
    if(err_dma || err_spi){
        printk(KERN_ALERT"ERROR INITIALIZING RGBLED PCI DEVICE: err_dma: %d , err_spi: %d",err_dma, err_spi);
        return -1;
    }

    if ( devices.dma_dev.dma_bar == 0){
        printk (KERN_ALERT"RGBLED driver cannot take possession of DMA/SPI device. BAR DMA = 0x%x :: BAR SPI = 0x%x  \n",devices.dma_dev.dma_bar,devices.spi_dev.spi_bar);    
        printk (KERN_ALERT"Try unload/blacklist dw_dmac_pci and spi_pxa2xx_spi/pxa2xx_spi_pci driver before proceed.");
        return -1;
    }
    printk(KERN_INFO"PCI DEVICE BAR: 0x%x  \n",devices.dma_dev.dma_bar);
    printk(KERN_INFO"PCI DEVICE BAR: 0x%x  \n",devices.spi_dev.spi_bar);

    return 0;

}

void __exit cleanup_module(){

    pci_unregister_driver(&pdrv_dma);
    pci_unregister_driver(&pdrv_spi);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    printk(KERN_INFO"######################### GOODBYE ##############################\n");

}
