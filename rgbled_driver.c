    

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
#include "headers/rgbled.h"



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

#define WS281X_DMADESCMEMORY(lednum)    ((sizeof(dma_lli_t) * ((3 * lednum) + 2))  % 4096) > 0 ? (((sizeof(dma_lli_t) * ((3 * lednum) + 2))  / 4096)+1)*4096 : ((sizeof(dma_lli_t) * ((3 * lednum) + 2))  / 4096)*4096
#define APA102_DMADESCMEMORY(lednum)    ((sizeof(dma_lli_t) * (lednum + 2)) % 4096) > 0 ? (((sizeof(dma_lli_t) * (lednum + 2)) / 4096)+1)*4096 : ((sizeof(dma_lli_t) * (lednum + 2)) / 4096)*4096

devices_t    devices;
static      __u16           dma_desc_counter;
static      __u32           dma_desc_printer;
static      __u32           tmp_addr;
static      dma_addr_t      tmp_dma_addr_phys;
volatile    dma_lli_t       *tmp_desc =  NULL;
volatile    dma_lli_t       *tmp_desc_prv = NULL;
static      __u32           test_var = 0xffFFffFF;
static      __u32           test_array[2] = {0xFF00FF00, 0xFF00ff00};
__u32 *fifo_data;
/*  PCI VARIABLES   */
static      int     err;
static      __u8    ahb;        

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
            devices.dma_dev.dma_desc_mem = WS281X_DMADESCMEMORY(devices.rgbled_numleds);
        else
            devices.dma_dev.dma_desc_mem = APA102_DMADESCMEMORY(devices.rgbled_numleds);

        fifo_data = kmalloc(16, __GFP_DMA);
        *fifo_data = 0x10101f10;
        *(fifo_data + 1) = 0xffffffff;

        printk (KERN_INFO"DMA fifo data : 0x%llx  -- PTR: %p \n",virt_to_phys(fifo_data), fifo_data);
        printk (KERN_INFO"DMA array data : 0x%llx  -- PTR: %p \n",virt_to_phys(&test_array[0]), &test_array[0]);
        printk (KERN_INFO"DMA u32 var data : 0x%llx  -- PTR: %p \n",virt_to_phys(&test_var), &test_var);
        printk (KERN_INFO"DMA u32 sent by user : 0x%x  \n", devices.rgbled_numleds);      
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
        dma_desc_counter = 0;
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = 0x1;
        devices.dma_dev.dma_ch->__sar_l__ = 0x0;
        devices.dma_dev.dma_ch->__dar_l__ = SPI_BASE_ADDR+0x10;
        devices.dma_dev.dma_ch->__llp_l__ = 0x1;
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


            devices.dma_dev.dma_cfg->__dmacfgre_l__ = DMA_DMACFGREG_L_DMA_EN;
/*
    printk(KERN_ALERT"*****\tSPI REGISTERS\t*****\n\n");
    printk(KERN_ALERT"Control Reg 0\t\t%08x\n",devices.spi_dev.ssp_control_block->__sscr0__ );
    printk(KERN_ALERT"Control Reg 1\t\t%08x\n", devices.spi_dev.ssp_control_block->__sscr1__);
    printk(KERN_ALERT"Status Reg\t\t%08x\n", devices.spi_dev.ssp_control_block->__sssr__);
    printk(KERN_ALERT"Interrupt Reg\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssitr__);
    printk(KERN_ALERT"SSP Data Reg\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssdr__);
    printk(KERN_ALERT"Reserved 0\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[0]);
    printk(KERN_ALERT"Reserved 1\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[1]);
    printk(KERN_ALERT"Reserved 2\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[2]);
    printk(KERN_ALERT"Reserved 3\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[3]);
    printk(KERN_ALERT"Reserved 4\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[4]);
    printk(KERN_ALERT"SSP Time Out\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssto__);
    printk(KERN_ALERT"Programmable Serial\t%08x\n", devices.spi_dev.ssp_control_block->__sspsp__);
    printk(KERN_ALERT"TX Time Slot\t\t%08x\n", devices.spi_dev.ssp_control_block->__sstsa__);
    printk(KERN_ALERT"RX Time Slot\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssrsa__);
    printk(KERN_ALERT"Time Slot Status\t%08x\n", devices.spi_dev.ssp_control_block->__sstss__);
    printk(KERN_ALERT"Audio Clock Div\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssacd__);
    printk(KERN_ALERT"I2S Transmit FIFO\t%08x\n", devices.spi_dev.ssp_control_block->__itf__);
    printk(KERN_ALERT"SPI Transmit FIFO\t%08x\n", devices.spi_dev.ssp_control_block->__sitf__);
    printk(KERN_ALERT"SPI Receive FIFO\t%08x\n", devices.spi_dev.ssp_control_block->__sirf__);
    printk(KERN_ALERT"Private Clock\t\t%08x\n", devices.spi_dev.ssp_general_block->__prv_clock_params__);
    printk(KERN_ALERT"Software Reset\t\t%08x\n", devices.spi_dev.ssp_general_block->__resets__);
    printk(KERN_ALERT"General Purpose\t\t%08x\n", devices.spi_dev.ssp_general_block->__general__);
    printk(KERN_ALERT"SSP_REG\t\t\t%08x\n", devices.spi_dev.ssp_general_block->__ssp_reg__);
    printk(KERN_ALERT"SPI_CS_CTRL_REG\t\t%08x\n", devices.spi_dev.ssp_general_block->__spi_cs_ctrl__);
*/
    

        break;

    case IOCTL_RGBLED_DECONFIGURE:

        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        devices.dma_dev.dma_cfg->__reqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        devices.dma_dev.dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
    //    devices.dma_dev.dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
     //   devices.dma_dev.dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number));
        
        devices.spi_dev.ssp_control_block->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;
        devices.dma_dev.dma_cfg->__dmacfgre_l__ = 0;
        dma_desc_counter = 0;
        break;

    case IOCTL_RGBLED_RENDER:

    devices.spi_dev.ssp_control_block->__sscr0__ |= SPI_SSP_SSCR0_SSE_SSPENABLE;

        
        devices.dma_dev.dma_cfg->__chenreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        msleep(100);
        devices.dma_dev.dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
     //   devices.dma_dev.dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        msleep(100);

        devices.dma_dev.dma_cfg->__reqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        
      //  devices.dma_dev.dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + devices.dma_dev.dma_ch_number)) | (0x1 << devices.dma_dev.dma_ch_number);
        
        

    /*
    printk(KERN_INFO"PRINTING  :  %x\n",devices.spi_dev.ssp_control_block->__sssr__);
    printk(KERN_INFO"PRINTING  :  %x\n",devices.spi_dev.ssp_control_block->__ssdr__);
    */

        break;
    case IOCTL_RGBLED_SETAHBLAYER:
        get_user(ahb, (__u8 *)ioctl_params);
    break;

    case IOCTL_DMA_ADDITEM:
        get_user(tmp_addr, (__u32 *)ioctl_params);

        tmp_desc = ((volatile dma_lli_t *)(devices.dma_dev.dma_desc)) + dma_desc_counter;
        tmp_dma_addr_phys = virt_to_bus(devices.dma_dev.dma_desc) + ((sizeof(dma_lli_t) * dma_desc_counter)); //devices.dma_dev.dma_desc_phys + (sizeof(dma_lli_t) * dma_desc_counter);

        //printk(KERN_ALERT"ADDR RECEIVED: 0x%08x counter: %i  temp_desc: %p \n",tmp_addr,dma_desc_counter,tmp_desc);
        //printk(KERN_ALERT"virt_:to_bus: 0x%llx \n",tmp_dma_addr_phys);

        if ( dma_desc_counter == 0 ){
            devices.dma_dev.dma_ch->__sar_l__ = tmp_addr; //tmp_addr;
            devices.dma_dev.dma_ch->__dar_l__ = SPI_BASE_ADDR+0x10;
            devices.dma_dev.dma_ch->__llp_l__ = DMA_LLP_LO_ADDRESSOFNEXTLLP(tmp_dma_addr_phys) | 0x0;
         } 
        else {
            tmp_desc_prv = ((volatile dma_lli_t *)(devices.dma_dev.dma_desc)) + (dma_desc_counter - 1);
            tmp_desc_prv->__sar_l__ = tmp_addr;
            tmp_desc_prv->__dar_l__ = SPI_BASE_ADDR+0x10;
            tmp_desc_prv->__llp_l__ = DMA_LLP_LO_ADDRESSOFNEXTLLP(tmp_dma_addr_phys) |
                                      DMA_LLP_LO_AHBLAYEROFNEXTLLP(0x0);
            tmp_desc_prv->__ctl_l__ =
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

            tmp_desc_prv->__ctl_h__ =
                DMA_CTL_HI_DONE_DONEBITZERO |
                DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0x1) ;

        }

            tmp_desc->__sar_l__ = 0x0;
            tmp_desc->__dar_l__ = 0x0;
            tmp_desc->__llp_l__ = DMA_LLP_LO_ADDRESSOFNEXTLLP(0x0)|
                                  DMA_LLP_LO_AHBLAYEROFNEXTLLP(0x0);
            tmp_desc->__ctl_l__ =
                DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGDISABLE |
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
            tmp_desc->__ctl_h__ =
                DMA_CTL_HI_DONE_DONEBITZERO |
                DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0x1) ;

           dma_desc_counter++;

        break;

    case IOCTL_DMA_PRINTITEMS:
        printk(KERN_ALERT"DMA desc mem address: %p\n\n",devices.dma_dev.dma_desc);
        printk(KERN_ALERT"DMA mem size : %08x\n\n",devices.dma_dev.dma_desc_mem);
        printk(KERN_ALERT"DMA ch size : %lx\n\n",sizeof(*devices.dma_dev.dma_ch));
        printk(KERN_INFO"__chenreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__chenreg_l__ );
        printk(KERN_INFO"__dmacfgre_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__dmacfgre_l__ );
        printk(KERN_INFO"__reqdstreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__reqdstreg_l__);
        printk(KERN_INFO"__reqsrcreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__reqsrcreg_l__);
        printk(KERN_INFO"__sglrqdstreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__sglrqdstreg_l__);
        printk(KERN_INFO"__sglrqsrcreg_l__\t\t%08x\n", devices.dma_dev.dma_cfg->__sglrqsrcreg_l__);
                        

        dma_desc_printer = 0;
        
        while (  dma_desc_printer <= dma_desc_counter ){

            if ( dma_desc_printer == 0 ){
                printk(KERN_INFO"*****\tDMA Decriptor # %d  %p\t*****\n", dma_desc_printer, devices.dma_dev.dma_ch);
                printk(KERN_INFO"__sar_l__\t\t%08x\n", devices.dma_dev.dma_ch->__sar_l__);
                printk(KERN_INFO"__dar_l__\t\t%08x\n", devices.dma_dev.dma_ch->__dar_l__);
                printk(KERN_INFO"__llp_l__\t\t%08x :: 0x%08x\n", devices.dma_dev.dma_ch->__llp_l__,(devices.dma_dev.dma_ch->__llp_l__ >> 2));
                printk(KERN_INFO"__ctl_l__\t\t%08x\n", devices.dma_dev.dma_ch->__ctl_l__);
                printk(KERN_INFO"__ctl_h__\t\t%08x\n", devices.dma_dev.dma_ch->__ctl_h__);
                printk(KERN_INFO"__sstat_l__\t\t%08x\n", devices.dma_dev.dma_ch->__ssta_l__);
                printk(KERN_INFO"__dstat_l__\t\t%08x\n", devices.dma_dev.dma_ch->__dsta_l__);
                printk(KERN_INFO"__sstatar_l__\t\t%08x\n", devices.dma_dev.dma_ch->__sstatar_l__);
                printk(KERN_INFO"__dstatar_l__\t\t%08x\n", devices.dma_dev.dma_ch->__dstatar_l__);
                printk(KERN_INFO"__cfg_l__\t\t%08x\n", devices.dma_dev.dma_ch->__cfg_l__);
                printk(KERN_INFO"__cfg_h__\t\t%08x\n", devices.dma_dev.dma_ch->__cfg_h__);
                printk(KERN_INFO"__srg_l__\t\t%08x\n", devices.dma_dev.dma_ch->__sgr_l__);
                printk(KERN_INFO"__dsr_l__\t\t%08x\n\n", devices.dma_dev.dma_ch->__dsr_l__);
            }
            else{
                tmp_desc = ((volatile dma_lli_t *)(devices.dma_dev.dma_desc))+(dma_desc_printer-1);
                printk(KERN_INFO"*****\tDMA Decriptor # %d  %p\t*****\n", dma_desc_printer, tmp_desc);
                printk(KERN_INFO"__sar_l__\t\t%08x\n", tmp_desc->__sar_l__);
                printk(KERN_INFO"__dar_l__\t\t%08x\n", tmp_desc->__dar_l__);
                printk(KERN_INFO"__llp_l__\t\t%08x :: 0x%08x\n", tmp_desc->__llp_l__, (tmp_desc->__llp_l__ >> 2));
                printk(KERN_INFO"__ctl_l__\t\t%08x\n", tmp_desc->__ctl_l__);
                printk(KERN_INFO"__ctl_h__\t\t%08x\n", tmp_desc->__ctl_h__);

            }

            dma_desc_printer++;
        }
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

    /*  CREATE DMA MEMORY POOL FOR THE LLI's  */
    devices.dma_dev.dma_pool = pci_pool_create( "rgbled_devices.dma_dev.dma_pool", pdev, 1024, 32, 0 );
    devices.dma_dev.dma_desc = kmalloc(4096, __GFP_DMA); //pci_pool_alloc( devices.dma_dev.dma_pool, GFP_ATOMIC, &devices.dma_dev.dma_desc_phys);
    if (devices.dma_dev.dma_desc == NULL){
        err = -1;
        goto err_pool_alloc;
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
        //if(devices.rgbled_type == DEV_APA102)
            GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,1);
/*
printk(KERN_ALERT"*****\tSPI REGISTERS\t*****\n\n");
    printk(KERN_ALERT"Control Reg 0\t\t%08x\n",devices.spi_dev.ssp_control_block->__sscr0__ );
    printk(KERN_ALERT"Control Reg 1\t\t%08x\n", devices.spi_dev.ssp_control_block->__sscr1__);
    printk(KERN_ALERT"Status Reg\t\t%08x\n", devices.spi_dev.ssp_control_block->__sssr__);
    printk(KERN_ALERT"Interrupt Reg\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssitr__);
    printk(KERN_ALERT"SSP Data Reg\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssdr__);
    printk(KERN_ALERT"Reserved 0\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[0]);
    printk(KERN_ALERT"Reserved 1\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[1]);
    printk(KERN_ALERT"Reserved 2\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[2]);
    printk(KERN_ALERT"Reserved 3\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[3]);
    printk(KERN_ALERT"Reserved 4\t\t%08x\n", devices.spi_dev.ssp_control_block->__rsv_0x014__[4]);
    printk(KERN_ALERT"SSP Time Out\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssto__);
    printk(KERN_ALERT"Programmable Serial\t%08x\n", devices.spi_dev.ssp_control_block->__sspsp__);
    printk(KERN_ALERT"TX Time Slot\t\t%08x\n", devices.spi_dev.ssp_control_block->__sstsa__);
    printk(KERN_ALERT"RX Time Slot\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssrsa__);
    printk(KERN_ALERT"Time Slot Status\t%08x\n", devices.spi_dev.ssp_control_block->__sstss__);
    printk(KERN_ALERT"Audio Clock Div\t\t%08x\n", devices.spi_dev.ssp_control_block->__ssacd__);
    printk(KERN_ALERT"I2S Transmit FIFO\t%08x\n", devices.spi_dev.ssp_control_block->__itf__);
    printk(KERN_ALERT"SPI Transmit FIFO\t%08x\n", devices.spi_dev.ssp_control_block->__sitf__);
    printk(KERN_ALERT"SPI Receive FIFO\t%08x\n", devices.spi_dev.ssp_control_block->__sirf__);
    printk(KERN_ALERT"Private Clock\t\t%08x\n", devices.spi_dev.ssp_general_block->__prv_clock_params__);
    printk(KERN_ALERT"Software Reset\t\t%08x\n", devices.spi_dev.ssp_general_block->__resets__);
    printk(KERN_ALERT"General Purpose\t\t%08x\n", devices.spi_dev.ssp_general_block->__general__);
    printk(KERN_ALERT"SSP_REG\t\t\t%08x\n", devices.spi_dev.ssp_general_block->__ssp_reg__);
    printk(KERN_ALERT"SPI_CS_CTRL_REG\t\t%08x\n", devices.spi_dev.ssp_general_block->__spi_cs_ctrl__);

*/
    return 0;

    /*  ERROR HANDLER's  */
    err_iomap:
        iounmap(devices.gpio_dev.gpio_base);
        iounmap(devices.dma_dev.dma_base);
        iounmap(devices.spi_dev.spi_base);
    err_pool_alloc:
        //pci_pool_free(devices.dma_dev.dma_pool, devices.dma_dev.dma_desc, devices.dma_dev.dma_desc_phys);
        pci_pool_destroy(devices.dma_dev.dma_pool);
    err_set_dma_mask:
        pci_release_regions(pdev);
        pci_disable_device(pdev);
    err_request_regions:
    err_enable_device:

        return err;
}


static void pci_rgbled_remove (struct pci_dev *pdev){
    GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_mosi->__cfg__,0);
        //if(devices.rgbled_type == DEV_APA102)
    GPIO_CFG_FUNCTION(devices.gpio_dev.gpio_pin_spi_clk->__cfg__,0);


    /*  UNMAP IO MEMORY  */
    iounmap(devices.gpio_dev.gpio_base);
    iounmap(devices.dma_dev.dma_base);
    iounmap(devices.spi_dev.spi_base);

    /*  FREE DMA MEMORY POOL  */
    kfree(devices.dma_dev.dma_desc);
    //pci_pool_free(devices.dma_dev.dma_pool, devices.dma_dev.dma_desc, devices.dma_dev.dma_desc_phys);
    pci_pool_destroy(devices.dma_dev.dma_pool);
    
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
        printk (KERN_ALERT"RGBLED driver loaded BAR at 0x%x \n",devices.dma_dev.dma_bar);    
        printk (KERN_INFO"DMA descriptor handler: 0x%llx  -- PTR: %p \n",devices.dma_dev.dma_desc_phys,&devices.dma_dev.dma_desc_phys);
        printk (KERN_INFO"DMA cpu virtual address: 0x%p \n",devices.dma_dev.dma_desc);
        printk (KERN_INFO"SPI CTL BLK 1st reg: %x \n",devices.spi_dev.ssp_control_block->__sssr__);
        printk (KERN_INFO"SPI GEN BLK 1st reg: %x \n",devices.spi_dev.ssp_general_block->__prv_clock_params__);
        printk (KERN_INFO"DMA 1st reg: %x \n",devices.dma_dev.dma_ch->__ctl_l__);
        printk (KERN_INFO"GPIO 1st reg: %x \n",devices.gpio_dev.gpio_pin_spi_mosi->__cfg__);
        printk (KERN_INFO"test_ver p: %p  ::  virt_to_phys : %llx  ::  virt_to_bus : %llx \n",&test_var, virt_to_phys(&test_var), virt_to_bus(&test_var));
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
