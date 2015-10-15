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
	dmapage *next;
	dmapage *prev;
}_dma_item_;



/*****			DMA 			*****/

void INIT_DMA_LIST (_dma_item_ *page);


