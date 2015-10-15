#ifndef DMA_H_
#define DMA_H_
#endif /* DMA_H_ */



/*****			DMA BASE REGISTERS			*****/

#define 	DMA_BLOCK_SIZE		(4*1024)
#define 	DMA_BASE_ADDR		0x90818000
#define		DMA_CHANNEL0_OFF	(0x0  / 4)
#define		DMA_CHANNEL1_OFF	(0x58  / 4)
#define		DMA_CHANNEL2_OFF	(0xB0  / 4)
#define		DMA_CHANNEL3_OFF	(0x108  / 4)
#define		DMA_CHANNEL4_OFF	(0x160  / 4)
#define		DMA_CHANNEL5_OFF	(0x1B8  / 4)
#define		DMA_CHANNEL6_OFF	(0x210  / 4)
#define		DMA_CHANNEL7_OFF	(0x268  / 4)


/*****			PWM REGISTER OFFSETS			*****/

#define 	PWM0_CTRL_OFF	(0x0 / 4)
#define 	PWM0_RES_OFF	(804 / 4)
#define 	PWM0_GEN_OFF	(808 / 4)
#define 	PWM1_CTRL_OFF	(0x0 / 4)
#define 	PWM1_RES_OFF	(804 / 4)
#define 	PWM1_GEN_OFF	(808 / 4)


/*****			PWM STRUCTURES			*****/

typedef struct dmapage{
//	dmapage *__next__;
//	dmapage *__prev__;
}_dma_page_;

typedef struct{
	u_int32_t __sar_;
};

typedef struct{
	u_int32_t __SAR_L__;			// Source Address Register
	u_int32_t __SAR_H__;			// Reserved
	u_int32_t __DAR_L__;			// Destination Address Register
	u_int32_t __DAR_H__;			// Reserved
	u_int32_t __LLP_L__;			// Linked List Pointer Register
	#define DMA_LLP_L_LOC(value)		((value & 0xFFFFFFFF) << 2)		// Starting Address In Memory of next LLI if block chaining is enabled.
	#define DMA_LLP_L(value)			(value & 0xF)					// Identifies the AHB layer/interface where the memory device that stores the next linked list item resides.
	u_int32_t __LLP_H__;			// Reserved
	u_int32_t __CTL_L__;			// Control Register
	#define DMA_CTL_L_LLP_SRC_EN	(1 << 28)							// Block chaining is enabled on the source side only if the LLP_SRC_EN field is high and LLPx.LOC is non-zero
	#define DMA_CTL_L_LLP_DST_EN	(1 << 27)							// Block chaining is enabled on the destination side only if the LLP_DST_EN field is high and LLPx.LOC is non-zero.
	#define DMA_CTL_L_SMS(value)	((value & 0x3) << 25)				// Identifies the Master Interface layer from which the	source device (peripheral or memory) is accessed.
	#define DMA_CTL_L_DMS(value)
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	#define DMA_CTL_L
	u_int32_t __CTL_H__;			// Control Register 2
	u_int32_t __SSTAT_L__;			// Source Status Register
	u_int32_t __SSTAT_H__;			//
	u_int32_t __DSTAT_L__;			// Destination Status Register
	u_int32_t __DSTAT_H__;			//
	u_int32_t __SSTATAR_L__;		// Source Status Address Register
	u_int32_t __SSTATAR_H__;		//
	u_int32_t __DSTATAR_L__;		// Destination Status Address Register
	u_int32_t __DSTATAR_H__;		//
	u_int32_t __CFG_L__;			// Configuration Register
	u_int32_t __CFG_H__;			// Configuration Register 2
	u_int32_t __SGR_L__;			// Source Gather Register
	u_int32_t __SGR_H__;			//
	u_int32_t __DSR_L__;			// Destination Scatter Register
	u_int32_t __DSR_H__;			// 





	unsigned long 			__pwm_base_addr_ptr__;
	int 					__pwm_memory_file__;
	void 					*__pwm_memory_map__;
	volatile unsigned int 	*__pwm_memory_address__;
}_dma_channel_


/*****			DMA 			*****/
int 	MAP_DMA_CHANNEL		(u_int8_t ch_num);
void	UNMAP_DMA_CHANNEL	(u_int8_t ch_num);
void	PRINT_DMA_STATUS	(u_int8_t ch_num);
//void 	INIT_DMA_LIST 		(_dma_item_ *page);




