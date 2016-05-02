#ifndef DMA_H_
#define DMA_H_
#endif /* DMA_H_ */



/*****			DMA BASE REGISTERS			*****/

#define 	DMA_BASE_ADDR			0x90c04000 //90818000
#define 	DMA_CTL_L_RESERVED_MASK	0xE0080000
#define		DMA_CTL_H_RESERVED_MASK	0xFFFFE000
#define		DMA_CFG_L_RESERVED_MASK	0x0000001F
#define 	DMA_CFG_H_RESERVED_MASK	0xFFFF8000
#define		DMA_OFFSET_CH0			(0x0  / 4)
#define		DMA_OFFSET_CH1			(0x58  / 4)
#define		DMA_OFFSET_CH2			(0xB0  / 4)
#define		DMA_OFFSET_CH3			(0x108  / 4)
#define		DMA_OFFSET_CH4			(0x160  / 4)
#define		DMA_OFFSET_CH5			(0x1B8  / 4)
#define		DMA_OFFSET_CH6			(0x210  / 4)
#define		DMA_OFFSET_CH7			(0x268  / 4)
#define     DMA_DMACCFG_OFF			(0x360 / 4)

u_int32_t 	dma_channels[8] ;


/*****			DMA MACROS			*****/

#define DMA_CHANNEL_BASE(channel)			( channel * (0x58 / 4 ) )



/*****			DMA STRUCTURES			*****/

typedef struct{
	u_int32_t __sar_l__;						// Source Address Register
	u_int32_t __rsv_0x04__;						// Reserved
	u_int32_t __dar_l__;						// Destination Address Register
	u_int32_t __rsv_0x0c__;						// Reserved
	u_int32_t __llp_l__;						// Linked List Pointer Register
	#define DMA_LLP_LO_ADDRESSOFNEXTLLP(value)	(value << 2)				// Starting Address In Memory of next LLI if block chaining is enabled.
	#define DMA_LLP_LO_AHBLAYEROFNEXTLLP(value)	(value)						// Identifies the AHB layer/interface where the memory device that stores the next linked list item resides.
	u_int32_t __rsv_0x14__;						// Reserved

	u_int32_t __ctl_l__;						// Control Register
	#define DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGENABLE	(0 << 28)
	#define DMA_CTL_LO_LLPSRCEN_SRCLLPCHAINNINGDISABLE	(1 << 28)					// Block chaining is enabled on the source side only if the LLP_SRC_EN field is high and LLPx.LOC is non-zero
	#define DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGENABLE	(0 << 27)
	#define DMA_CTL_LO_LLPDSTEN_DSTLLPCHAINNINGDISABLE	(1 << 27)					// Block chaining is enabled on the destination side only if the LLP_DST_EN field is high and LLPx.LOC is non-zero.
	#define DMA_CTL_LO_SMS_SRCMASTERLAYER1				(0x0 << 25)		// Identifies the Master Interface layer from which the	source device (peripheral or memory) is accessed.
	#define DMA_CTL_LO_SMS_SRCMASTERLAYER2				(0x1 << 25)
	#define DMA_CTL_LO_SMS_SRCMASTERLAYER3				(0x2 << 25)
	#define DMA_CTL_LO_SMS_SRCMASTERLAYER4				(0x3 << 25)
	#define DMA_CTL_LO_SMS_SRCNOHARDCODE				(0x4 << 25)
	#define DMA_CTL_LO_DMS_DSTMASTERLAYER1				(0x0 << 23)
	#define DMA_CTL_LO_DMS_DSTMASTERLAYER2				(0x1 << 23)
	#define DMA_CTL_LO_DMS_DSTMASTERLAYER3				(0x2 << 23)
	#define DMA_CTL_LO_DMS_DSTMASTERLAYER4				(0x3 << 23)
	#define DMA_CTL_LO_DMS_DSTNOHARDCODE				(0x4 << 23)
	#define DMA_CTL_LO_TTFC_FLOWCONTROLBYDMA			(0x0 << 20)
	#define DMA_CTL_LO_TTFC_FLOWCONTROLBYSOURCE			(0x1 << 20)
	#define DMA_CTL_LO_TTFC_FLOWCONTROLBYDEST			(0x2 << 20)
	#define DMA_CTL_LO_TTFC_FLOWCONTROLBYANYDEVICE		(0x3 << 20)
	#define DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERDISABLE	(0 << 18)
	#define DMA_CTL_LO_DSTSCATTEREN_DESTSCATTERENABLE	(1 << 18)
	#define DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERDISABLE	(0 << 17)
	#define DMA_CTL_LO_SRCGATHEREN_SOURCEGATHERENABLE	(1 << 17)					// 0 - Disabled; 1 - Enabled
	#define DMA_CTL_LO_SRCMSIZE_SRCBURSTITEMNUM(value)	(value << 14)				// Number of data items, each of width CTLx.SRC_TR_WIDTH, to be read from the source every time a source burst transaction request is made from either the corresponding hardware or software handshaking interface.
	#define DMA_CTL_LO_DESTMSIZE_DSTBURSTITEMNUM(value)	(value << 11)				// Number of data items, each of width CTLx.DST_TR_WIDTH, to be written to the destination every time a destination burst transaction request is made from either the corresponding hardware or software handshaking interface.
	#define DMA_CTL_LO_SINC_SOURCEADDRINCREMENT			(0x0 << 9)			// 00 - Increment, 01 - Decrement, 1x - No Change
	#define DMA_CTL_LO_SINC_SOURCEADDRDECREMENT			(0x1 << 9)
	#define DMA_CTL_LO_SINC_SOURCEADDRNOCHANGE			(0x2 << 9)
	#define DMA_CTL_LO_DINC_DESTADDRINCREMENT			(0x0 << 7)			// 00 - Increment, 01 - Decrement, 1x - No Change
	#define DMA_CTL_LO_DINC_DESTADDRDECREMENT			(0x1 << 7)
	#define DMA_CTL_LO_DINC_DESTADDRNOCHANGE			(0x2 << 7)
	#define DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF8BITS	(0x0 << 4) //8 bits				// This value must be less than or equal to DMAH_Mx_HDATA_WIDTH, where x is the AHB layer 1 to 4 where the source resides
	#define DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF16BITS	(0x1 << 4) //16 bits
	#define DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF32BITS	(0x2 << 4) //32 bits
	#define DMA_CTL_LO_SRCTRWIDTH_SRCTRANSFEROF64BITS	(0x3 << 4) //64 bits
	#define DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF8BITS	(0x0 << 1)				// This value must be less than or equal to DMAH_Mx_HDATA_WIDTH, where x is the AHB layer 1 to 4 where the source resides
	#define DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF16BITS	(0x1 << 1)
	#define DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF32BITS	(0x2 << 1)
	#define DMA_CTL_LO_DSTTRWIDTH_DSTTRANSFEROF64BITS	(0x3 << 1)
	#define DMA_CTL_LO_INTEN_INTERRUPTSDISABLED			(0x0)					// If set, then all interrupt-generating sources are enabled.
	#define DMA_CTL_LO_INTEN_INTERRUPTSENABLED			(0x1)

	u_int32_t __ctl_h__;						// Control Register 2
	#define DMA_CTL_HI_DONE_DONEBITZERO					(0 << 12)					// Software can poll the LLI CTLx.DONE bit to see when a block transfer is complete.
	#define DMA_CTL_HI_DONE_DONEBITONE					(1 << 12)
	#define DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(value)	(value)						//  When the DW_ahb_dmac is the flow controller, the user writes this field before the channel is enabled, to indicate the block size
	u_int32_t __ssta_l__;						// Source Status Register
	u_int32_t __rsv_0x24;						// Reserved
	u_int32_t __dsta_l__;						// Destination Status Register
	u_int32_t __rsv_0x2c;						// Reserved
	u_int32_t __sstatar_l__;					// Source Status Address Register
	u_int32_t __rsv_0x34__;						// Reserved
	u_int32_t __dstatar_l__;					// Destination Status Address Register
	u_int32_t __rsv_0x3c__;						// Reserved
	u_int32_t __cfg_l__;						// Configuration Register
	#define DMA_CFG_LO_RELOADDST_NORELOADDSTAFTERTRANSFER	(0 << 31)					// The DARx register can be automatically reloaded from its initial value at the end of every block for multi-block transfers.
	#define DMA_CFG_LO_RELOADDST_RELOADDSTAFTERTRASNFER		(1 << 31)
	#define DMA_CFG_LO_RELOADSRC_NORELOADSRCAFTERTRANSFER	(0 << 30)					// The SARx register can be automatically reloaded from its initial value at the end of every block for multi-block transfers.
	#define	DMA_CFG_LO_RELOADSRC_RELOADSRCAFTERTRANSFER		(1 << 30)
	#define DMA_CFG_LO_MAXABRST_MAXBURSTTRANSFER(value)		(value << 20)				// Maximum AMBA burst length that is used for DMA transfers on this channel. A value of 0 indicates that software is not limiting the maximum AMBA burst length
	#define DMA_CFG_LO_MAXABRST_NOLIMITBURSTTRANSFER		(0x0  << 20)
	#define DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVEHIGH		(0 << 19)
	#define DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVELOW		(1 << 19)					// 0 - Active High; 1 - Active Low
	#define DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVEHIGH		(0 << 18)
	#define DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVELOW		(1 << 18)					// 0 - Active High; 1 - Active Low
	#define DMA_CFG_LO_LOCKB_BUSNOTLOCKED					(0 << 17)
	#define DMA_CFG_LO_LOCKB_BUSLOCKDURINGLOCK_B_L			(1 << 17)					// When active, the AHB bus master signal hlock is asserted for the duration specified in CFGx.LOCK_B_L.
	#define DMA_CFG_LO_LOCKCH_CHANNELNOTLOCKED				(0 << 16)
	#define DMA_CFG_LO_LOCKCH_CHANNELLOCKDURINGLOCK_CH_L	(1 << 16)					// When the channel is granted control of the master bus interface and if the CFGx.LOCK_CH bit is asserted, then no other channels are granted control of the master bus interface for the duration specified in CFGx.LOCK_CH_L.
	#define DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSFER		(0x0 << 14)
	#define DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMABLOCKTRANSF	(0x1 << 14)
	#define DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSACTION	(0x2 << 14)				// Indicates the duration over which CFGx.LOCK_B bit applies. 00 - Over complete DMA transfer; 01 - Over complete block transfer; 1x - Over complete DMA transaction
	#define DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSFER		(0x0 << 12)
	#define DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMABLOCKTRANSF	(0x1 << 12)
	#define DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSACTION	(0x2 << 12)				// Indicates the duration over which CFGx.LOCK_CH bit applies. 00 - DMA transfer; 01 - DMA block transfer; 1x - DMA trasaction.
	#define DMA_CFG_LO_HSSELSRC_SOURCEHWHANDSHAKING			(0x0 << 11)
	#define DMA_CFG_LO_HSSELSRC_SOURCESWHANDSHAKING			(0x1 << 11)					// 0 - HW handshaking; 1 - SW handshaking
	#define DMA_CFG_LO_HSSELDST_DESTHWHANDSHAKING			(0x0 << 10)
	#define DMA_CFG_LO_HSSELDST_DESTSWHANDSHAKING			(0x1 << 10)					// 0 - HW handshaking; 1 - SW handshaking
	#define DMA_CFG_LO_FIFOEMPTY_SETVALUEZERO				(0x0 << 9)
	#define DMA_CFG_LO_FIFOEMPTY_CHECKVALUE					(0x1 << 9)					// Indicates whether there is data left in the channel FIFO.
	#define DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY	(0x0 << 8)
	#define DMA_CFG_LO_CHSUSP_SUSPENDDMACHANNELACTIVITY		(0x1 << 8)					// Suspends all DMA transfers from source until this bit is cleared.
	#define DMA_CFG_LO_CHPRIOR_LOWESTPRIORITY				(0x0 << 5)
	#define DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY				(0x7 << 5)
	#define	DMA_CFG_LO_CHPRIOR_SETPRIORITY(value)			(value << 5)				// Priority of 7 is the highest priority.

	u_int32_t __cfg_h__;						// Configuration Register 2
	#define	DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(value)	(value << 11)				// Assigns a hardware handshaking interface (0 - DMAH_NUM_HS_INT-1) to the destination of channel x if the CFGx.HS_SEL_DST field is 0; otherwise, this field is ignored.
	#define DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(value)	(value << 7)				// Assigns a hardware handshaking interface (0 - DMAH_NUM_HS_INT-1) to the source of channel x if the CFGx.HS_SEL_SRC field is 0; otherwise, this field is ignored.
	#define DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE		(0 << 6)
	#define DMA_CFG_HI_SSUPDEN_ENABLESRCSTATUSUPDATE		(1 << 6)					// Source status information is fetched only from the location pointed to by the SSTATARx register, stored in the SSTATx register and written out to the SSTATx location of the LLI if SS_UPD_EN is high.
	#define DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE		(0 << 5)					// Destination status information is fetched only from the location pointed to by the DSTATARx register, stored in the DSTATx register and written out to the DSTATx location of the LLI if DS_UPD_EN is high.
	#define DMA_CFG_HI_DSUPDEN_ENABLEDSTSTATUSUPDATE		(1 << 5)
	#define DMA_CFG_HI_PROTCTL(value)						(value << 2)				// Protection Control bits used to drive the AHB HPROT[3:1] bus.
	#define DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH		(0 << 1)					// Determines how much space or data needs to be available in the FIFO before a burst transaction request is serviced. 0 - Space/data available for single AHB transfer of the specified transfer width; 1 - Data available is greater than or equal to half the FIFO depth for destination transfers
	#define DMA_CFG_HI_FIFOMODE_DATASPACEHALFOFFIFODSTSRC	(1 << 1)
	#define DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR		(0)					// Determines when source transaction requests are serviced when the Destination Peripheral is the flow controller. 0 - Source transaction requests are serviced when they occur. Data pre-fetching is enabled; 1 - Source transaction requests are not serviced until a destination transaction request occurs.
	#define DMA_CFG_HI_FCMODE_SRCTRANSREQUNTILDATAREQOCURR	(1)

	u_int32_t __sgr_l__;						// Source Gather Register
	#define DMA_SGR_LO_SGC_SRCGATHERCOUNTMASK				(0xFFF << 20)				// Source contiguous transfer count between successive gather boundaries.
	#define DMA_SGR_LO_SGI_SRCGATHERINCDECMULTIPLE(value)	(value)						// Specifies the source address increment/decrement in multiples of CTLx.SRC_TR_WIDTH on a gather boundary when gather mode is enabled for the source transfer.
	u_int32_t __rsv_0x4c__;						// Reserved
	u_int32_t __dsr_l__;						// Destination Scatter Register
	#define DMA_DSR_LO_DSC_DESTSCATTERCOUNTMASK				(0xFFF << 20)				// Destination contiguous transfer count between successive scatter boundaries.
	#define DMA_DSR_LO_DSI_DESTSCATTERINCDECMULTIPLE(value)	(value)				// Specifies the destination address increment/decrement in multiples of CTLx.DST_TR_WIDTH on a scatter boundary, when scatter mode is enabled for the destination transfer.
}__attribute__ ((packed)) dma_channel_t;


typedef struct {
	u_int32_t __statusint_l__;
	u_int32_t __rsv_0x364__;
	u_int32_t __reqsrcreg_l__;
	u_int32_t __rsv_0x36c__;
	u_int32_t __reqdstreg_l__;
	u_int32_t __rsv_0x374__;
	u_int32_t __sglrqsrcreg_l__;
	u_int32_t __rsv_0x37c__;
	u_int32_t __sglrqdstreg_l__;
	u_int32_t __rsv_0x384__;
	u_int32_t __lstsrcreg_l__;
	u_int32_t __rsv_0x38c__;
	u_int32_t __lstdstreg_l__;
	u_int32_t __rsv_0x394__;
	u_int32_t __dmacfgre_l__;					// DW_ahb_dmac Configuration Register
	#define DMA_DMACFGREG_L_DMA_EN				(0x1)					// 0 - DW_ahb_dmac Disabled; 1 - DW_ahb_dmac Enabled	
	u_int32_t __resv_0x39c__;
	u_int32_t __chenreg_l__;				// DW_ahb_dmac Channel Enable Register
	#define DMA_DMACHENREG_L_CH_EN_WE(value)	(value<<8)		// This register enables the CH_EN to be written in the bit choosen
	#define DMA_DMACHENREG_L_CH_EN(value)		(value)			// 0 - Disable the channel; 1 - Enable the channel
	u_int32_t __rsv_0x3a4__[10];
	u_int32_t __dmaparamsch7__;
	#define DMA_DMAPARAMSCH_FIFODEPTH			(0x7 << 28)
	#define DMA_DMAPARAMSCH_SMS					(0x7 << 25)
	#define DMA_DMAPARAMSCH_LMS					(0x7 << 22)
	#define DMA_DMAPARAMSCH_DMS					(0x7 << 19)
	#define DMA_DMAPARAMSCH_MAXMULTSIZE			(0x7 << 16)
	#define DMA_DMAPARAMSCH_FC					(0x3 << 14)
	#define DMA_DMAPARAMSCH_HCLLP				(0x1 << 13)
	#define DMA_DMAPARAMSCH_CTLWBEN				(0x1 << 12)
	#define DMA_DMAPARAMSCH_MULTIBLKEN			(0x1 << 11)
	#define DMA_DMAPARAMSCH_LOCKEN				(0x1 << 10)
	#define DMA_DMAPARAMSCH_SRCGATEN			(0x1 << 9)
	#define DMA_DMAPARAMSCH_DSTSCAEN			(0x1 << 8)
	#define DMA_DMAPARAMSCH_STATSRC				(0x1 << 7)
	#define DMA_DMAPARAMSCH_STATDST				(0x1 << 6)
	#define DMA_DMAPARAMSCH_STW					(0x7 << 3)
	#define DMA_DMAPARAMSCH_DTW					(0x7)
	u_int32_t __dmaparamsch6__;
	u_int32_t __dmaparamsch5__;
	u_int32_t __dmaparamsch4__;
	u_int32_t __dmaparamsch3__;
	u_int32_t __dmaparamsch2__;
	u_int32_t __dmaparamsch1__;
	u_int32_t __dmaparamsch0__;
}__attribute__ ((packed)) dma_cfg_t;


typedef struct dma_lli{
	struct dma_lli *prev;
	struct dma_lli *next;
	void *address;
}_dma_lli_t_;


/*****			DMA 			*****/
int print_dma_status (u_int32_t channel_reg);
/*
void 			INIT_DMA_LIST 		(_dma_lli_t_ *head);
_dma_lli_t_		*ADD_DMA_ITEM		(_dma_lli_t_ *head);
_dma_lli_t_ 	*NEXT_DMA_ITEM 		(_dma_lli_t_ *lli);
_dma_lli_t_ 	*PREV_DMA_ITEM 		(_dma_lli_t_ *lli);
void			PRINT_DMA_STATUS	(u_int8_t ch_num);

void 	INIT_DMA_LIST 		(_dma_item_ *page);
*/



