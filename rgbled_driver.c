    

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
#include "rgbled_driver.h"
#include "headers/rgbled_devices.h"



MODULE_LICENSE("GPL");
MODULE_AUTHOR("ernestrc");
MODULE_DESCRIPTION("MinnowboardMAX APA102/WS281X module.");
MODULE_SUPPORTED_DEVICE("MinnowboardMAX");
MODULE_INFO(intree, "Y");

/*** Transmission control defines ***/

#define RGBLED_SPI_FIFOTHRESHOLD                (60)
#define RGBLED_DMA_BLOCK_SIZE                   (0x80)
#define RGBLED_DMA_POOL_SIZE(bytes)             (bytes % 1024 ? ((bytes / 1024) + 1) * 1024 : bytes)
#define RGBLED_DATA_SIZE_WS281X(lednum)         (sizeof(led_ws281x_t) * lednum )
#define RGBLED_DATA_SIZE_APA102(lednum)         (sizeof(led_apa102_t) * lednum )
#define RBGLED_DMA_ITEMLISTNUMBER(datasize)     (datasize % (RGBLED_DMA_BLOCK_SIZE * 4) ? ((datasize / (RGBLED_DMA_BLOCK_SIZE * 4)) + 1) : (datasize / (RGBLED_DMA_BLOCK_SIZE * 4)))
#define RGBLED_DMA_DATABLOCKREMAINDER(datasize) ((datasize % (RGBLED_DMA_BLOCK_SIZE * 4) / 4))

/*** Device variables ***/

static int open = 0 ;
static dev_t first;         
static struct cdev c_dev;   
static struct class *cl;    

int k;

/*** PCI variables ***/
static int err_dma, err_spi;

/*** Module Variables ***/

devices_t devices;

static __u32 i;   
static __u32 result;
static __u32 spiRxData;
static __u32 ssdr; 
static __u8 finish_flag = 0;
static dma_item_t *head;


/*** SSP configuration variables ***/
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
            SPI_SSP_SSCR1_TRAIL_CPUHANDLETRAILINGBYTE |   
            SPI_SSP_SSCR1_TSRE_DMATRANSMITREQDISABLE |    
            SPI_SSP_SSCR1_RSRE_DMARECEIVEREQDISABLE |     
            SPI_SSP_SSCR1_TINTE_NOINTERRUPTONTIMEOUT |
            SPI_SSP_SSCR1_PINTE_NOTRAILINGBYTEINTERRUPT |
            SPI_SSP_SSCR1_IFS_FRAMEPOLARITYBYTHEFORMAT |
            SPI_SSP_SSCR1_RFT_RECEIVEFIFOTRIGTHRESHOLD(0x9) |
            SPI_SSP_SSCR1_TFT_TRANSMITFIFOTRIGTHRESHOLD(0xC) |
            SPI_SSP_SSCR1_TIE_TRANSFIFOLEVELINTERRENA |   
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

/*** DMA TX channel configuration variables ***/
__u32 dma_tx_ctl_l_cfg =
            DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGDISABLE |
            DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE |
            DMA_CTL_LO_SMS_SRCMASTERLAYER1 |
            DMA_CTL_LO_DMS_DSTMASTERLAYER1 |
            DMA_CTL_LO_TTFC_FLOWCONTROLBYDMA |
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
            DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(RGBLED_DMA_BLOCK_SIZE) ; 
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
            DMA_CFG_LO_HSSELSRC_SOURCESWHANDSHAKING |        
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

/*** DMA RX channel configuration variables ***/
__u32 dma_rx_ctl_l_cfg =
            DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGDISABLE |
            DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE |
            DMA_CTL_LO_SMS_SRCMASTERLAYER1 |
            DMA_CTL_LO_DMS_DSTMASTERLAYER1 |
            DMA_CTL_LO_TTFC_FLOWCONTROLBYDMA |
            DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERDISABLE |
            DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERDISABLE |
            DMA_CTL_LO_SRCMSIZE_SRCBURSTITEMNUM(0x7) |
            DMA_CTL_LO_DESTMSIZE_DSTBURSTITEMNUM(0x7) |
            DMA_CTL_LO_SINC_SOURCEADDRNOCHANGE |
            DMA_CTL_LO_DINC_DESTADDRNOCHANGE |
            DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF32BITS |
            DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF32BITS |
            DMA_CTL_LO_INTEN_INTERRUPTSENABLED ;
__u32 dma_rx_ctl_h_cfg = 
            DMA_CTL_HI_DONE_DONEBITZERO |
            DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0xFF0) ; 
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
            DMA_CFG_LO_HSSELSRC_SOURCEHWHANDSHAKING |           
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
*****************   Module char device functions    ***********************
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
*****************     Interrupt callbacks         *************************
***************************************************************************/


static irqreturn_t short_spi_interrupt(int irq, void *dev_id)
{
    spi_dev_t *spi_dev = (spi_dev_t *)dev_id;
    __u32 fifo_level = (spi_dev->ssp_control_block->__sitf__ & SPI_SSP_SITF_SITFL_READTXFIFOENTRIESSPI) >> 16 ;

    /* If the TX FIFO is below the limit set, TFS flag will activate. 
    This flag can only be cleared until the FIFO is empty. Due this limitant 
    is needed to poll the FIFO level manually and request a DMA transfer, which 
    should be ready from the DMA interrupt callback */
    if ( spi_dev->ssp_control_block->__sssr__ & SPI_SSP_SSSR_TFS_TRANSMITFIFOSERVICEREQ ){
                
        /* If SSP SPI FIFO is below the FIFO threshold check if an additional DMA request is needed */
        if ( fifo_level < RGBLED_SPI_FIFOTHRESHOLD ){

            /* If the finish flag is set end the data transmission */
            if( finish_flag ){
                devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1)));
                /* Flush the SSP SPI RX FIFO to finish the data transmission */
                do {
                     while ( spi_dev->ssp_control_block->__sssr__ & SPI_SSP_SSSR_RNE_RECEIVEFIFONOTEMPTY) {
                             ssdr = spi_dev->ssp_control_block->__ssdr__;
                     }
                } while ( spi_dev->ssp_control_block->__sssr__ & SPI_SSP_SSSR_BSY_SPIBUSY );    
                
                /* Deactivate SSP SPI on closure */
                devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1)));
                spi_dev->ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;
                
                 /* Clear interrupt flags once the FIFOs are empty */
                spi_dev->ssp_control_block->__sssr__ &= ssp_sssr_clearint_mask;
             
            /* If finish flag is not set, transfer the next block */
            } else {
                devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
            }
        }
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

    
static irqreturn_t short_dma_interrupt(int irq, void *dev_id)
{
    dma_dev_t *dma_dev = (dma_dev_t *)dev_id;
    __u32 status = dma_dev->dma_cfg->__statusint__;

    /* Confirm the interruption is provoked by a DMA event */ 
    if ( status & 0x1F ){
            
            /* TX DMA channel interrupt */
            if ( dma_dev->dma_cfg->__rawtfr__ & DMA_INT_STATUSREG_MASK(devices.dma_dev.dma_ch_number)){
                
                /* Check if this is the last transmission */
                if(head->next != NULL){
                    head = head->next;
                    
                    /* Reconfigure TX DMA Channel */
                    dma_dev->dma_tx_ch->__sar_l__ = head->src_addr;
                    dma_dev->dma_tx_ch->__dar_l__ = SPI_BASE_ADDR + 0x10;
                    dma_dev->dma_tx_ch->__llp_l__ = 0x0;
                    dma_dev->dma_tx_ch->__ctl_l__ = dma_tx_ctl_l_cfg;
                    dma_dev->dma_tx_ch->__ctl_h__ = head->blk_size;
                    dma_dev->dma_tx_ch->__cfg_l__ = dma_tx_cfg_l_cfg;
                    dma_dev->dma_tx_ch->__cfg_h__ = dma_tx_cfg_h_cfg;
                
                }else{
                    finish_flag++;
                }
            }

            /* TX DMA channel interrupt */
            if ( dma_dev->dma_cfg->__rawtfr__ & DMA_INT_STATUSREG_MASK(devices.dma_dev.dma_ch_number + 1)){
                
                /* Reconfigure RX FIFO DMA channel */
                devices.dma_dev.dma_rx_ch->__sar_l__ = SPI_BASE_ADDR + 0x10;
                devices.dma_dev.dma_rx_ch->__dar_l__ = (uintptr_t)(&spiRxData);
                devices.dma_dev.dma_rx_ch->__llp_l__ = 0x0;
                devices.dma_dev.dma_rx_ch->__ctl_l__ = dma_rx_ctl_l_cfg;
                devices.dma_dev.dma_rx_ch->__ctl_h__ = dma_rx_ctl_h_cfg;
                devices.dma_dev.dma_rx_ch->__cfg_l__ = dma_rx_cfg_l_cfg;
                devices.dma_dev.dma_rx_ch->__cfg_h__ = dma_rx_cfg_h_cfg;
                devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));
            }
       
       /* Clear DMA Interrupts */
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__cleartfr__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__clearblock__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__clearsrctran__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__cleardsttran__));
        __raw_writel(0xFF , &(dma_dev->dma_cfg->__clearerr__ ));

        /* Refresh DMA Interrupt Masks - Only activate transmission complete interrupt */
        __raw_writel(0xFFFF , &(dma_dev->dma_cfg->__masktfr__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__maskblock__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__masksrctran__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__maskdsttran__));
        __raw_writel(0xFF00 , &(dma_dev->dma_cfg->__maskerr__ ));

        
        return IRQ_HANDLED;
    }
    
    return IRQ_NONE;    
}



/**************************************************************************
*****************     IOCTL functions          ****************************
***************************************************************************/

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_params){
    switch (ioctl_num){

    /*** IOCTL - Set module configuration ***/
    case IOCTL_RGBLED_SETCONFIG:
        get_user(devices.rgbled_config, (__u32 *)ioctl_params );

        /*** Initial Configuration ***/

        /* Set DMA channel to use */
        devices.dma_dev.dma_ch_number = RGBLED_CONF_GET_DMACH(devices.rgbled_config);
        
        /* Set DMA total transmission size - In Bytes */
        devices.dma_dev.dma_data_size = (RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config)) ? 
                                        RGBLED_DATA_SIZE_APA102 (RGBLED_CONF_GET_LEDNUM(devices.rgbled_config)): 
                                        RGBLED_DATA_SIZE_WS281X (RGBLED_CONF_GET_LEDNUM(devices.rgbled_config));
        
        /*** Memory Allocation ***/

        /*  Allocate the PCI memory pool for the TX DMA data  */
        devices.dma_dev.dma_pool = pci_pool_create( "rgbled_dma_pool", devices.pdev_dma, RGBLED_DMA_POOL_SIZE(devices.dma_dev.dma_data_size), 32, 0 );
        devices.dma_dev.dma_data_ptr = pci_pool_alloc( devices.dma_dev.dma_pool, GFP_ATOMIC, &devices.dma_dev.dma_data_phys); 
        if (devices.dma_dev.dma_data_ptr == NULL){
            printk(KERN_ALERT"Cannot allocate pci memory block for DMA data, please release memory and try again");
        }

        /* "memset" DMA data memory */
        for(i = 0 ; i < devices.dma_dev.dma_data_size ; i++)
            *((__u8 *)devices.dma_dev.dma_data_ptr + i) = 0x00;    
        
        /*** GPIO pin configuration ***/

        /* If SPI MOSI is already configured, leave it */
        if(!(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__ & 0x1))
            GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__,1);
        /* Configure SPI CLK if APA102 LEDs are used */
        if(RGBLED_CONF_GET_LEDTYPE(devices.rgbled_config))
            if(!(devices.gpio_dev.gpio_pin_spi_clk->__cfg__ & 0x1))    
                GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,1);
    
        /* MMIO mapping for DMA */
        
        /* Map DMA channel registers to a local pointer */
        devices.dma_dev.dma_tx_ch = (volatile dma_ch_t *)(devices.dma_dev.dma_base + dma_channels[devices.dma_dev.dma_ch_number]);
        devices.dma_dev.dma_rx_ch = (volatile dma_ch_t *)(devices.dma_dev.dma_base + dma_channels[devices.dma_dev.dma_ch_number+1]);

        /*** DMA transmission list configuration ***/
        
        /* Allocate the needed memory */
        devices.dma_dev.dma_list_itemnumber = RBGLED_DMA_ITEMLISTNUMBER(devices.dma_dev.dma_data_size);
        devices.dma_dev.dma_list = kmalloc(sizeof(dma_item_t) * devices.dma_dev.dma_list_itemnumber, GFP_HIGHUSER);

        /* Build DMA transmission list */
        for ( i = 0 ; i < devices.dma_dev.dma_list_itemnumber ; i++ ){
            devices.dma_dev.dma_list[i].src_addr = devices.dma_dev.dma_data_phys + (RGBLED_DMA_BLOCK_SIZE * 4 * i);
            devices.dma_dev.dma_list[i].blk_size = ((i + 1) >= devices.dma_dev.dma_list_itemnumber ? 
                                                            RGBLED_DMA_DATABLOCKREMAINDER(devices.dma_dev.dma_data_size):
                                                            RGBLED_DMA_BLOCK_SIZE );
            if(i > 0)
              devices.dma_dev.dma_list[i-1].next = &(devices.dma_dev.dma_list[i]);
            
            devices.dma_dev.dma_list[i].next = NULL;
        }
        
        /*** Prepare devices prior rendering ***/

        /* Ensure DMA controller is up */
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = DMA_DMACFGREG_L_DMA_ENA;
        
        /* Deactivate DMA channels */
        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1)));

        /* Deactivate SSP SPI */
        devices.spi_dev.ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;

        break;


    /*** IOCTL - Configuration - Deprecated, will be used to other functions ***/

    case IOCTL_RGBLED_CONFIGURE:
        
        break;

    /*** IOCTL - Deconfigure - Deprecated, will be used to other functions ***/

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

    /*** IOCTL - Render RGB leds ***/

    case IOCTL_RGBLED_RENDER:
        
        /*** Data preparation ***/

        /* TODO - Clear to add X11 information */
        rgbled_test(devices.dma_dev.dma_data_ptr, RGBLED_CONF_GET_LEDNUM(devices.rgbled_config));
        __clear_cache((char *)devices.dma_dev.dma_data_ptr,
                        (char *)(devices.dma_dev.dma_data_ptr + 5 ));

        /* Point head to the first DMA item and reset the finish flag */
        head = devices.dma_dev.dma_list;
        finish_flag = 0;

        /*** Device Configuration ***/

        /* Deactivate DMA channels */
        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1)));

        /* Deactivate SSP SPI */
        devices.spi_dev.ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;

        /* Configuring SPI */
        devices.spi_dev.ssp_control_block->__sscr0__ = ssp_sscr0_cfg; 
        devices.spi_dev.ssp_control_block->__sscr1__ = ssp_sscr1_cfg; 
        devices.spi_dev.ssp_control_block->__sssr__ &= ssp_sssr_clearint_mask;
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

        /* Configuring TX DMA */
        devices.dma_dev.dma_tx_ch->__sar_l__ = head->src_addr;
        devices.dma_dev.dma_tx_ch->__dar_l__ = SPI_BASE_ADDR + 0x10;
        devices.dma_dev.dma_tx_ch->__llp_l__ = 0x0;
        devices.dma_dev.dma_tx_ch->__ctl_l__ = dma_tx_ctl_l_cfg;
        devices.dma_dev.dma_tx_ch->__ctl_h__ = head->blk_size;
        devices.dma_dev.dma_tx_ch->__cfg_l__ = dma_tx_cfg_l_cfg;
        devices.dma_dev.dma_tx_ch->__cfg_h__ = dma_tx_cfg_h_cfg;
        
        /* Configuring RX DMA */
        devices.dma_dev.dma_rx_ch->__sar_l__ = SPI_BASE_ADDR + 0x10;
        devices.dma_dev.dma_rx_ch->__dar_l__ = (uintptr_t)(&spiRxData);
        devices.dma_dev.dma_rx_ch->__llp_l__ = 0x0;
        devices.dma_dev.dma_rx_ch->__ctl_l__ = dma_rx_ctl_l_cfg;
        devices.dma_dev.dma_rx_ch->__ctl_h__ = dma_rx_ctl_h_cfg;
        devices.dma_dev.dma_rx_ch->__cfg_l__ = dma_rx_cfg_l_cfg;
        devices.dma_dev.dma_rx_ch->__cfg_h__ = dma_rx_cfg_h_cfg;
        
        /* Clear interrupts and add mask for transfer complete interrupt */ 
        __raw_writel(0xFF , &(devices.dma_dev.dma_cfg->__cleartfr__));
        __raw_writel(0xFF , &(devices.dma_dev.dma_cfg->__clearblock__));
        __raw_writel(0xFF , &(devices.dma_dev.dma_cfg->__clearsrctran__));
        __raw_writel(0xFF , &(devices.dma_dev.dma_cfg->__cleardsttran__));
        __raw_writel(0xFF , &(devices.dma_dev.dma_cfg->__clearerr__ ));
        
        __raw_writel(0xFFFF , &(devices.dma_dev.dma_cfg->__masktfr__));
        __raw_writel(0xFF00 , &(devices.dma_dev.dma_cfg->__maskblock__));
        __raw_writel(0xFF00 , &(devices.dma_dev.dma_cfg->__masksrctran__));
        __raw_writel(0xFF00 , &(devices.dma_dev.dma_cfg->__maskdsttran__));
        __raw_writel(0xFF00 , &(devices.dma_dev.dma_cfg->__maskerr__ ));

        /* Clear RX FIFO */        
        while ( devices.spi_dev.ssp_control_block->__sssr__ & SPI_SSP_SSSR_RNE_RECEIVEFIFONOTEMPTY) {
            ssdr = devices.spi_dev.ssp_control_block->__ssdr__;
        }

        /*** Transmission start ***/

        /* Enable SSP SPI */
        devices.spi_dev.ssp_control_block->__sscr0__ |= SPI_SSP_SSCR0_SSE_SSPENABLE;    
        
        /* Enable DMA TX and RX DMA channels */
        devices.dma_dev.dma_cfg->__chenreg_l__ = 
                        (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number)
                        | (0x1 << (8 + (devices.dma_dev.dma_ch_number + 1))) | (0x1 << (devices.dma_dev.dma_ch_number + 1));

        break;
    
    /*** IOCTL - DMA print items - Deprecated, to be deleted ***/

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

    break;

    }
    return 0;
}

/**************************************************************************
*****************     PCI functions      **********************************
***************************************************************************/
static int pci_rgbled_spi_probe (struct pci_dev *pdev, const struct pci_device_id *id){

    /*  Register PCI device  */
    err_spi = pci_enable_device(pdev);
    if (err_spi)
        goto err_enable_spi_device;

    err_spi = pci_request_regions(pdev, "rgbled_spi_module");
    if (err_spi)
        goto err_request_spi_regions;

    /* Get SPI PCI device data from the kernel */
    devices.pdev_spi = pci_dev_get(pdev);

    /*  Get SPI base address register */
    devices.spi_dev.spi_bar = pci_resource_start (pdev, 0);         

    /*  Get SPI base address register size */
    devices.spi_dev.spi_bar_size = pci_resource_len (pdev, 0);      

    /* Enable SPI as PCI master devices */
    pci_set_master(pdev);                           

    
    /*  Map MMIO for GPIO and SPI   */
    if (!(devices.gpio_dev.gpio_base = (volatile uint32_t *) ioremap(GPIO_SCORE_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"rgbled: GPIO MMIO mapping failed!\n");
        err_spi = -2;
        goto err_spi_iomap;
    }

    if (!(devices.spi_dev.spi_base = (volatile uint32_t *) ioremap(SPI_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"rgbled: SPI MMIO mapping failed!\n");
        err_spi = -2;
        goto err_spi_iomap;
    }

    devices.gpio_dev.gpio_pin_spi_clk = (volatile gpio_t *)(devices.gpio_dev.gpio_base + gpio_pins[11]);
    devices.gpio_dev.gpio_pin_spi_mosi = (volatile gpio_t *)(devices.gpio_dev.gpio_base + gpio_pins[9]);
    devices.spi_dev.ssp_control_block = (volatile ssp_control_t *)devices.spi_dev.spi_base;
    devices.spi_dev.ssp_general_block = (volatile ssp_general_t *)(devices.spi_dev.spi_base + SPI_SSP_GENERAL_OFFSET);
    
    /* Request a shared IRQ to the kernel */
    devices.spi_dev.spi_irqn = SPI_IRQn;
    
    if (devices.spi_dev.spi_irqn >= 0) {
        result = request_irq(devices.spi_dev.spi_irqn, short_spi_interrupt, 
                             IRQF_SHARED, "rgbled_spi_module", &(devices.spi_dev));
        if (result) {
            printk(KERN_ALERT"rgbled: Cannot request IRQ for SPI! - %i\n", devices.spi_dev.spi_irqn);
            devices.spi_dev.spi_irqn = -1;
            goto err_spi_iomap;
        }
        else { 
             printk(KERN_INFO"rgbled: SPI interrupt registered\n");
        }
    }

    return 0;

    /*  Error Handlers  */
    err_spi_iomap:
        iounmap(devices.gpio_dev.gpio_base);
        iounmap(devices.spi_dev.spi_base);
    
    err_request_spi_regions:
    err_enable_spi_device:

    return err_spi;
}

static int pci_rgbled_dma_probe (struct pci_dev *pdev, const struct pci_device_id *id){

    /*  Register PCI device  */
    err_dma = pci_enable_device(pdev);
    if (err_dma)
        goto err_enable_dma_device;

    err_dma = pci_request_regions(pdev, "rgbled_dma_module");
    if (err_dma)
        goto err_request_dma_regions;

    err_dma = pci_set_dma_mask(pdev, DMA_BIT_MASK(29));
    if (err_dma)
        goto err_set_dma_mask;

    err_dma = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(29));
    if (err_dma)
        goto err_set_dma_mask;

    /*  Get DMA base address register */ 
    devices.pdev_dma = pci_dev_get(pdev);
    devices.dma_dev.dma_bar = pci_resource_start (pdev, 0);  

    /*  Get DMA base address register size */ 
    devices.dma_dev.dma_bar_size = pci_resource_len (pdev, 0);   

    /*  Enable device as PCI master */   
    pci_set_master(pdev);                          

    /* Map MMIO for DMA */
    if (!(devices.dma_dev.dma_base = (volatile uint32_t *) ioremap(DMA_BASE_ADDR, BLOCK_SIZE_T))) {
        printk(KERN_ALERT"rgbled: DMA MMIO mapping failed!\n");
        err_dma = -2;
        goto err_dma_iomap;
    }
    
    /* Map DMA general registers to a device pointer, channels will be allocated on IOCTL_RGBLED_SETCONFIG */
    devices.dma_dev.dma_cfg = (volatile dma_cfg_t *)(devices.dma_dev.dma_base + DMA_DMACCFG_OFF);
    
    /* Allocate DMA Interrupt */
    devices.dma_dev.dma_irqn = DMA_IRQn;
    
    if (devices.dma_dev.dma_irqn >= 0) {
        result = request_irq(devices.dma_dev.dma_irqn, short_dma_interrupt,
                             IRQF_SHARED, "rgbled_dma_module", &(devices.dma_dev));
        if (result) {
            printk(KERN_ALERT"rgbled: Cannot request IRQ for DMA! - %i\n", devices.dma_dev.dma_irqn);
            devices.dma_dev.dma_irqn = -1;
            goto err_dma_iomap;
        }
        else {
        // actually enable it -- assume this *is* a parallel port 
             printk(KERN_INFO"rgbled: DMA interrupt registered\n");
        }
    }


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
    
    /*  Unmap MMIO */
    kfree(devices.dma_dev.dma_list);
    free_irq(devices.spi_dev.spi_irqn, &(devices.spi_dev));
    
    iounmap(devices.gpio_dev.gpio_base);
    iounmap(devices.spi_dev.spi_base);

    /*  Unregister PCI device  */
    pci_dev_put(pdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

}

static void pci_rgbled_dma_remove (struct pci_dev *pdev){
    free_irq(devices.dma_dev.dma_irqn, &(devices.dma_dev));

    /*  Unmap MMIO */
    iounmap(devices.dma_dev.dma_base);
    
       /*  Free DMA memory pool  */
    pci_pool_free(devices.dma_dev.dma_pool, devices.dma_dev.dma_data_ptr, devices.dma_dev.dma_data_phys);
    pci_pool_destroy(devices.dma_dev.dma_pool);
 
    /*  Unregister PCI device */
    pci_dev_put(pdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

}


/*** Supported devices structure ***/

static struct pci_device_id pci_supp_dma_list[] = {
    { PCI_VDEVICE(INTEL, 0x0f06)},
    {0,},
};

static struct pci_device_id pci_supp_spi_list[] = {
    { PCI_VDEVICE(INTEL, 0x0f0e)},
    {0,},
};
/*** Export supported devices to user space ***/
MODULE_DEVICE_TABLE(pci, pci_supp_dma_list);
MODULE_DEVICE_TABLE(pci, pci_supp_spi_list);

/*** Register module ***/
static struct pci_driver pdrv_dma = {
    .name       = "rgbled_dma_module",
    .id_table   = pci_supp_dma_list,
    .remove     = pci_rgbled_dma_remove,
    .probe      = pci_rgbled_dma_probe,
};

static struct pci_driver pdrv_spi = {
    .name       = "rgbled_spi_module",
    .id_table   = pci_supp_spi_list,
    .remove     = pci_rgbled_spi_remove,
    .probe      = pci_rgbled_spi_probe,
};



/**************************************************************************
*****************     Init / Cleanup functions          *******************
***************************************************************************/

struct file_operations fop = {
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release
};


int __init init_module(){
    printk(KERN_INFO"rgbled: MinnowboardMax SPI-DMA rgbled module\n");


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
        printk(KERN_ALERT"rgbled: Error registering module ::: err_dma: %d , err_spi: %d\n",err_dma, err_spi);
        return -1;
    }

    if ( devices.dma_dev.dma_bar == 0){
        printk (KERN_ALERT"rgbled: Module cannot take possession of DMA and/or SPI devices\n");    
        printk (KERN_ALERT"rgbled: Try to unload/blacklist dw_dmac_pci and spi_pxa2xx_spi/pxa2xx_spi_pci modules before proceed\n");
        return -1;
    }
    printk(KERN_INFO"rgbled: DMA base address on 0x%08x  \n",devices.dma_dev.dma_bar);
    printk(KERN_INFO"rgbled: SPI base address on 0x%08x  \n",devices.spi_dev.spi_bar);

    return 0;

}

void __exit cleanup_module(){

    pci_unregister_driver(&pdrv_dma);
    pci_unregister_driver(&pdrv_spi);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    printk(KERN_INFO"rgbled: Module unload. Goodbye\n");

}
