#ifndef DMA_H_
#define DMA_H_
#endif /* DMA_H_ */



/*****			DMA BASE REGISTERS			*****/

#define 	DMA_BASE_ADDR		0x90818000
#define		DMA_OFFSET_CH0		(0x0  / 4)
#define		DMA_OFFSET_CH1		(0x58  / 4)
#define		DMA_OFFSET_CH2		(0xB0  / 4)
#define		DMA_OFFSET_CH3		(0x108  / 4)
#define		DMA_OFFSET_CH4		(0x160  / 4)
#define		DMA_OFFSET_CH5		(0x1B8  / 4)
#define		DMA_OFFSET_CH6		(0x210  / 4)
#define		DMA_OFFSET_CH7		(0x268  / 4)
#define     DMA_DMACCFG_OFF		(0x398 / 4)

u_int32_t 	dma_channels[] = {
DMA_OFFSET_CH0,
DMA_OFFSET_CH1,
DMA_OFFSET_CH2,
DMA_OFFSET_CH3,
DMA_OFFSET_CH4,
DMA_OFFSET_CH5,
DMA_OFFSET_CH6,
DMA_OFFSET_CH7
};


/*****			DMA MACROS			*****/

//#define		DMA_MAP(base_pointer, dma_num)		(volatile dma_ch_t *)(base_pointer + dma_channels[##dma_num ]);



/*****			DMA STRUCTURES			*****/

typedef struct{
	#define DMA_CHANNEL_BASE(channel)			( channel * (0x58 / 4 ) )
	u_int32_t __sar_l__;						// Source Address Register
	u_int32_t __rsv_0x04__;						// Reserved
	u_int32_t __dar_l__;						// Destination Address Register
	u_int32_t __rsv_0x0c__;						// Reserved
	u_int32_t __llp_l__;						// Linked List Pointer Register
	#define DMA_LLP_L_LOC(value)				(value << 2)				// Starting Address In Memory of next LLI if block chaining is enabled.
	#define DMA_LLP_L(value)					(value)						// Identifies the AHB layer/interface where the memory device that stores the next linked list item resides.
	u_int32_t __rsv_0x14__;						// Reserved
	u_int32_t __ctl_l__;						// Control Register
	#define DMA_CTL_L_LLP_SRC_EN				(1 << 28)					// Block chaining is enabled on the source side only if the LLP_SRC_EN field is high and LLPx.LOC is non-zero
	#define DMA_CTL_L_LLP_DST_EN				(1 << 27)					// Block chaining is enabled on the destination side only if the LLP_DST_EN field is high and LLPx.LOC is non-zero.
	#define DMA_CTL_L_SMS(value)				(value & 0x3) << 25)		// Identifies the Master Interface layer from which the	source device (peripheral or memory) is accessed.
	#define DMA_CTL_L_DMS(value)				(value & 0x3) << 23)		// Identifies the Master Interface layer where the destination device (peripheral or memory) resides.
	#define DMA_CTL_L_TT_FC(value)				(value << 20)				// Config Mem-to-Mem, Mem-to-Peri, Pery-to-Mem or Peri-to-Peri. Flow Control can be assigned to the DW_ahb_dmac, the source peripheral, or the destination peripheral.
	#define DMA_CTL_L_DST_SCATTER_EN			(1 << 18)					// 0 - Disabled; 1 - Enabled
	#define DMA_CTL_L_SRC_GATHER_EN				(1 << 17)					// 0 - Disabled; 1 - Enabled
	#define DMA_CTL_L_SRC_MSIZE(value)			(value << 14)				// Number of data items, each of width CTLx.SRC_TR_WIDTH, to be read from the source every time a source burst transaction request is made from either the corresponding hardware or software handshaking interface.
	#define DMA_CTL_L_DEST_MSIZE(value)			(value << 11)				// Number of data items, each of width CTLx.DST_TR_WIDTH, to be written to the destination every time a destination burst transaction request is made from either the corresponding hardware or software handshaking interface.
	#define DMA_CTL_L_SINC(value)				(value & 0x3) << 9)			// 00 - Increment, 01 - Decrement, 1x - No Change
	#define DMA_CTL_L_DINC(value)				(value & 0x3) << 7)			// 00 - Increment, 01 - Decrement, 1x - No Change
	#define DMA_CTL_L_SRC_TR_WIDTH(value)		(value << 4)				// This value must be less than or equal to DMAH_Mx_HDATA_WIDTH, where x is the AHB layer 1 to 4 where the source resides
	#define DMA_CTL_L_DST_TR_WIDTH(value)		(value << 1)				// This value must be less than or equal to DMAH_Mk_HDATA_WIDTH, where k is the AHB layer 1 to 4 where the destination resides.
	#define DMA_CTL_L_INT_EN					(1 << 0)					// If set, then all interrupt-generating sources are enabled.
	u_int32_t __ctl_h__;						// Control Register 2
	#define DMA_CTL_H_DONE						(1 << 12)					// Software can poll the LLI CTLx.DONE bit to see when a block transfer is complete. 
	#define DMA_CTL_H_BLOCK_TS(value)			(value)						//  When the DW_ahb_dmac is the flow controller, the user writes this field before the channel is enabled, to indicate the block size
	u_int32_t __ssta_l__;						// Source Status Register
	u_int32_t __rsv_0x24;						// Reserved
	u_int32_t __dsta_l__;						// Destination Status Register
	u_int32_t __rsv_0x2c;						// Reserved
	u_int32_t __sstatar_l__;					// Source Status Address Register
	u_int32_t __rsv_0x34__;						// Reserved
	u_int32_t __dstatar_l__;					// Destination Status Address Register
	u_int32_t __rsv_0x3c__;						// Reserved
	u_int32_t __cfg_l__;						// Configuration Register
	#define DMA_CFG_L_RELOAD_DST				(1 << 31)					// The DARx register can be automatically reloaded from its initial value at the end of every block for multi-block transfers.
	#define DMA_CFG_L_RELOAD_SRC				(1 << 30)					// The SARx register can be automatically reloaded from its initial value at the end of every block for multi-block transfers.
	#define	DMA_CFG_L_MAX_ABRST(value)			(value << 20)				// Maximum AMBA burst length that is used for DMA transfers on this channel. A value of 0 indicates that software is not limiting the maximum AMBA burst length 
	#define DMA_CFG_L_SRC_HS_POL				(1 << 19)					// 0 - Active High; 1 - Active Low
	#define DMA_CFG_L_DST_HS_POL				(1 << 18)					// 0 - Active High; 1 - Active Low
	#define DMA_CFG_L_LOCK_B					(1 << 17)					// When active, the AHB bus master signal hlock is asserted for the duration specified in CFGx.LOCK_B_L.
	#define DMA_CFG_L_LOCK_CH					(1 << 16)					// When the channel is granted control of the master bus interface and if the CFGx.LOCK_CH bit is asserted, then no other channels are granted control of the master bus interface for the duration specified in CFGx.LOCK_CH_L. 
	#define DMA_CFG_L_LOCK_B_L(value)			(value << 14)				// Indicates the duration over which CFGx.LOCK_B bit applies. 00 - Over complete DMA transfer; 01 - Over complete block transfer; 1x - Over complete DMA transaction
	#define DMA_CFG_L_LOCK_CH_L(value)			(value << 12)				// Indicates the duration over which CFGx.LOCK_CH bit applies. 00 - DMA transfer; 01 - DMA block transfer; 1x - DMA trasaction.
	#define DMA_CFG_L_HS_SEL_SRC				(1 << 11)					// 0 - HW handshaking; 1 - SW handshaking
	#define DMA_CFG_L_HS_SEL_DST				(1 << 10)					// 0 - HW handshaking; 1 - SW handshaking
 	#define DMA_CFG_L_FIFO_EMPTY				(1 << 9)					// Indicates whether there is data left in the channel FIFO.
 	#define DMA_CFG_L_CH_SUSP					(1 << 8)					// Suspends all DMA transfers from source until this bit is cleared.
 	#define	DMA_CFG_L_CH_PRIOR(value)			(value << 5)				// Priority of 7 is the highest priority.
	u_int32_t __cfg_h__;						// Configuration Register 2
	#define	DMA_CFG_H_DEST_PER(value)			(value << 11)				// Assigns a hardware handshaking interface (0 - DMAH_NUM_HS_INT-1) to the destination of channel x if the CFGx.HS_SEL_DST field is 0; otherwise, this field is ignored.
	#define DMA_CFG_H_SRC_PER(value)			(value << 7)				// Assigns a hardware handshaking interface (0 - DMAH_NUM_HS_INT-1) to the source of channel x if the CFGx.HS_SEL_SRC field is 0; otherwise, this field is ignored.
	#define DMA_CFG_H_SS_UPD_EN					(1 << 6)					// Source status information is fetched only from the location pointed to by the SSTATARx register, stored in the SSTATx register and written out to the SSTATx location of the LLI if SS_UPD_EN is high.
	#define DMA_CFG_H_DS_UPD_EN					(1 << 5)					// Destination status information is fetched only from the location pointed to by the DSTATARx register, stored in the DSTATx register and written out to the DSTATx location of the LLI if DS_UPD_EN is high.
	#define DMA_CFG_H_PROTCTL(value)			(value << 2)				// Protection Control bits used to drive the AHB HPROT[3:1] bus. 
	#define DMA_CFG_H_FIFO_MODE					(1 << 1)					// Determines how much space or data needs to be available in the FIFO before a burst transaction request is serviced. 0 - Space/data available for single AHB transfer of the specified transfer width; 1 - Data available is greater than or equal to half the FIFO depth for destination transfers
	#define DMA_CFG_H_FCMODE					(1 << 0)					// Determines when source transaction requests are serviced when the Destination Peripheral is the flow controller. 0 - Source transaction requests are serviced when they occur. Data pre-fetching is enabled; 1 - Source transaction requests are not serviced until a destination transaction request occurs. 
	u_int32_t __srg_l__;						// Source Gather Register
	#define DMA_SGR_L_SGC(value)				(value << 20)				// Source contiguous transfer count between successive gather boundaries.
	#define DMA_SGR_L_SGI(value)				(value)						// Specifies the source address increment/decrement in multiples of CTLx.SRC_TR_WIDTH on a gather boundary when gather mode is enabled for the source transfer.
	u_int32_t __rsv_0x4c__;						// Reserved
	u_int32_t __dsr_l__;						// Destination Scatter Register
	#define DMA_DSR_L_DSC(value)				(value << 20)				// Destination contiguous transfer count between successive scatter boundaries.
	#define DMA_DSR_L_DSI(value)				(value)						// Specifies the destination address increment/decrement in multiples of CTLx.DST_TR_WIDTH on a scatter boundary, when scatter mode is enabled for the destination transfer.
}dma_channel_t;


typedef struct {
	u_int32_t __dmacfgre_l__;					// DW_ahb_dmac Configuration Register
	#define DMA_DMACFGREG_L_DMA_EN				(0x1)					// 0 - DW_ahb_dmac Disabled; 1 - DW_ahb_dmac Enabled	
	u_int32_t __resv_0x39c__;
	u_int32_t __chenreg_l__;				// DW_ahb_dmac Channel Enable Register
	#define DMA_DMACHENREG_L_CH_EN_WE(value)	(1 << (value + 8))		// This register enables the CH_EN to be written in the bit choosen		
	#define DMA_DMACHENREG_L_CH_EN(value)		(1 << value)			// 0 - Disable the channel; 1 - Enable the channel
}dma_cfg_t;


typedef struct dma_lli{
	struct dma_lli *prev;
	struct dma_lli *next;
	void *address;
}_dma_lli_t_;


/*****			DMA 			*****/
/*
void 			INIT_DMA_LIST 		(_dma_lli_t_ *head);
_dma_lli_t_		*ADD_DMA_ITEM		(_dma_lli_t_ *head);
_dma_lli_t_ 	*NEXT_DMA_ITEM 		(_dma_lli_t_ *lli);
_dma_lli_t_ 	*PREV_DMA_ITEM 		(_dma_lli_t_ *lli);
void			PRINT_DMA_STATUS	(u_int8_t ch_num);

void 	INIT_DMA_LIST 		(_dma_item_ *page);
*/



