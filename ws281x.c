/*
 * ws281x.c
 *
 * WS281x Functionality
 * The basic functionality is to set the clock of the PWM as close as possible 
 * to the frequency specified in the WS281x documentation.
 * A FIFO is created to contain the registers of the PWM_ON_TIME_DIVISOR which
 * will be varying to draw the logical bits in the PWM signal.
 * 
 * My assumption is that the PWM Controller contains a FIFO which will be fill
 * with the data from the DMA transfer.
 *
 *  Created on: Oct 28, 2015
 *      Author: minnow
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/pci.h>
#include "headers/ws281x.h"
#include "headers/general.h"
#include "headers/pwm.h"
#include "headers/dma.h"
#include "headers/gpio.h"
#include "headers/spi.h"

#define BYTECOUNT(led_num) 		(24*led_num*4)   	//24 registers per led... Each register of 4 bytes (u_int32_t)
#define WORDCOUNT(led_num)		(24*led_num)

typedef struct ws281x_devices {
	volatile pwm_t *pwm_dev;
	volatile dma_channel_t *dma_ch;
	volatile dma_cfg_t *dma_cfg;
	volatile gpio_t *gpio_pin_spi_mosi;
	volatile gpio_t *gpio_pin_spi_clk;
	volatile u_int32_t *fifo_data;
	volatile u_int32_t *fifo_data2;
	volatile ssp_control_t *ssp_control_block;
	volatile ssp_general_t *ssp_general_block;
} ws281x_devices_t;

static u_int32_t ws281x_convert_virtual	(const volatile void *addr) {
	char filename[40];
	u_int64_t page_frame_buffer;
	int file_descriptor;

	sprintf(filename, "/proc/%d/pagemap", getpid());

	file_descriptor = open(filename, O_RDONLY);							// Open the pagemap of the process.
	if (file_descriptor < 0) {
		perror("Cannot open pagemap file.");
		return (u_int32_t)~0UL;
	}

	u_int32_t offset = ((u_int32_t) addr /  BLOCK_SIZE) * 8 ;			// Formule to get the offset inside the pagemap.
	u_int32_t reminder = (u_int32_t)addr % BLOCK_SIZE;					// Reminder of the virtual address, will be added to the physical addres to point the memory.


	if (lseek(file_descriptor, offset, SEEK_SET) != offset) {
		perror("Failed lseek of the current offset.");
		close(file_descriptor);
		return (u_int32_t)~0UL;
	}

	if (read(file_descriptor, &page_frame_buffer, sizeof(page_frame_buffer)) != sizeof(page_frame_buffer)) {
		perror("addr_to_bus() read() failed");
		close(file_descriptor);
		return (u_int32_t)~0UL;
	}

	close(file_descriptor);
	printf("BEFORE STRIP%32x\n", page_frame_buffer);

	page_frame_buffer &= 0x7fffffffffffff; 									// We are only interested in the first 54 bits.
	page_frame_buffer <<= 12;												// Need to move bits left 12 times to get the correct boundary.
	page_frame_buffer += reminder;											// We add the reminder offset.
	printf("AFTER STRIP%32x\n", page_frame_buffer);
	return (u_int32_t) page_frame_buffer;
}

void ws281x_print_registers(ws281x_t *ws281x) {
	volatile gpio_t *gpio_clk = ws281x->devices->gpio_pin_spi_clk;
	volatile gpio_t *gpio_mosi = ws281x->devices->gpio_pin_spi_mosi;
	volatile pwm_t *pwm = ws281x->devices->pwm_dev;
	volatile dma_channel_t *dma = ws281x->devices->dma_ch;
	volatile dma_cfg_t *dma_cfg = ws281x->devices->dma_cfg;
	volatile ssp_control_t *ssp_ctrl = ws281x->devices->ssp_control_block;
	volatile ssp_general_t *ssp_gral = ws281x->devices->ssp_general_block;

	printf("###################################\n");
	printf("##                               ##\n");
	printf("##         REGISTERS             ##\n");
	printf("##                               ##\n");
	printf("###################################\n\n");

	printf("*****\tGPIO CLK REGISTERS\t*****\n\n");
	printf("__cfg__\t\t%08x\n", gpio_clk->__cfg__);
	printf("__val__\t\t%08x\n\n", gpio_clk->__val__);

	printf("*****\tGPIO MOSI REGISTERS\t*****\n\n");
	printf("__cfg__\t\t%08x\n", gpio_mosi->__cfg__);
	printf("__val__\t\t%08x\n\n", gpio_mosi->__val__);

	printf("*****\tPWM REGISTERS\t*****\n\n");
	printf("__pwmctrl__\t\t%08x\n\n", pwm->__pwmctrl__);

	printf("*****\tDMA REGISTERS\t*****\n\n");
	printf("__sar_l__\t\t%08x\n", dma->__sar_l__);
	printf("__dar_l__\t\t%08x\n", dma->__dar_l__);
	printf("__llp_l__\t\t%08x\n", dma->__llp_l__);
	printf("__ctl_l__\t\t%08x\n", dma->__ctl_l__);
	printf("__ctl_h__\t\t%08x\n", dma->__ctl_h__);
	printf("__sstat_l__\t\t%08x\n", dma->__ssta_l__);
	printf("__dstat_l__\t\t%08x\n", dma->__dsta_l__);
	printf("__sstatar_l__\t\t%08x\n", dma->__sstatar_l__);
	printf("__dstatar_l__\t\t%08x\n", dma->__dstatar_l__);
	printf("__cfg_l__\t\t%08x\n", dma->__cfg_l__);
	printf("__cfg_h__\t\t%08x\n", dma->__cfg_h__);
	printf("__srg_l__\t\t%08x\n", dma->__sgr_l__);
	printf("__dsr_l__\t\t%08x\n\n", dma->__dsr_l__);

	printf("*****\tDMA CONTROL REGISTERS\t*****\n\n");
	printf("__statusint_l__\t\t%08x\n", dma_cfg->__statusint_l__);
	printf("__reqsrcreg_l__\t\t%08x\n", dma_cfg->__reqsrcreg_l__);
	printf("__reqdstreg_l__\t\t%08x\n", dma_cfg->__reqdstreg_l__);
	printf("__sglrqsrcreg_l__\t%08x\n", dma_cfg->__sglrqsrcreg_l__);
	printf("__sglrqdstreg_l__\t%08x\n", dma_cfg->__sglrqdstreg_l__);
	printf("__lstsrcreg_l__\t\t%08x\n", dma_cfg->__lstsrcreg_l__);
	printf("__lstdstreg_l__\t\t%08x\n", dma_cfg->__lstdstreg_l__);
	printf("__dmacfgreg_l__\t\t%08x\n", dma_cfg->__dmacfgre_l__);
	printf("__chenreg_l__\t\t%08x\n\n", dma_cfg->__chenreg_l__);

	printf("*****\tSPI REGISTERS\t*****\n\n");
	printf("Control Reg 0\t\t%08x\n",ssp_ctrl->__sscr0__ );
	printf("Control Reg 1\t\t%08x\n", ssp_ctrl->__sscr1__);
	printf("Status Reg\t\t%08x\n", ssp_ctrl->__sssr__);
	printf("Interrupt Reg\t\t%08x\n", ssp_ctrl->__ssitr__);
	printf("SSP Data Reg\t\t%08x\n", ssp_ctrl->__ssdr__);
	printf("SSP Time Out\t\t%08x\n", ssp_ctrl->__ssto__);
	printf("Programmable Serial\t%08x\n", ssp_ctrl->__sspsp__);
	printf("TX Time Slot\t\t%08x\n", ssp_ctrl->__sstsa__);
	printf("RX Time Slot\t\t%08x\n", ssp_ctrl->__ssrsa__);
	printf("Time Slot Status\t%08x\n", ssp_ctrl->__sstss__);
	printf("Audio Clock Div\t\t%08x\n", ssp_ctrl->__ssacd__);
	printf("I2S Transmit FIFO\t%08x\n", ssp_ctrl->__itf__);
	printf("SPI Transmit FIFO\t%08x\n", ssp_ctrl->__sitf__);
	printf("SPI Receive FIFO\t%08x\n", ssp_ctrl->__sirf__);
	printf("Private Clock\t\t%08x\n", ssp_gral->__prv_clock_params__);
	printf("Software Reset\t\t%08x\n", ssp_gral->__resets__);
	printf("General Purpose\t\t%08x\n", ssp_gral->__general__);
	printf("SSP_REG\t\t\t%08x\n", ssp_gral->__ssp_reg__);
	printf("SPI_CS_CTRL_REG\t\t%08x\n", ssp_gral->__spi_cs_ctrl__);
}

void ws281x_print_fifo(ws281x_t *ws281x) {
	volatile u_int32_t *word = &((volatile u_int32_t *) ws281x->devices->fifo_data)[0];
	volatile u_int32_t *word2 = &((volatile u_int32_t *) ws281x->devices->fifo_data2)[0];
	printf("%08x\n", &ws281x->devices->fifo_data);
	printf("%p\n", ws281x->devices->fifo_data);
	printf("%08x\n", (volatile u_int32_t)word);

	printf("FIFO 1 \t\tFIFO 2\n");

	for (int i = 0; i < WORDCOUNT(ws281x->lednumber); i++)
		printf("%08x\t%08x\n", *(word + (i)), *(word2 + (i)));
}

int ws281x_fifo_init(ws281x_t *ws281x) {
	led_t *ledarray = ws281x->ledarray;

	//Allocate memory for the led color data
	ws281x->devices->fifo_data = malloc(BYTECOUNT(ws281x->lednumber));
	ws281x->devices->fifo_data2 = malloc(BYTECOUNT(ws281x->lednumber));
	if (!ws281x->devices->fifo_data)
		return -1;

	volatile u_int32_t *word = &((volatile u_int32_t *) ws281x->devices->fifo_data)[0];
	//memset(word, 0, BYTECOUNT(ws281x->lednumber));

	for (int i = 0; i < ws281x->lednumber; i++) {
		int j = 24 * i;
		*(word + (j++)) = ((*ledarray).r & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 2) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 4) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 8) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 16) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 32) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 64) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).r & 128) ?	WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 2) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 4) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 8) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 16) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 32) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 64) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).g & 128) ?	WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 2) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 4) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 8) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 16) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 32) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 64) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = ((*ledarray).b & 128) ?	WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;

	}

	return 0;
}

int ws281x_dma_setup(ws281x_t *ws281x) {
	volatile dma_channel_t *dma = ws281x->devices->dma_ch;

	dma->__sar_l__ = ws281x_convert_virtual(ws281x->devices->fifo_data);
	dma->__dar_l__ = ws281x_convert_virtual(ws281x->devices->fifo_data2);//(u_int32_t)0x90825000;
	dma->__ctl_l__ &= DMA_CTL_L_RESERVED_MASK;   //SAVE RESERVED VALUES JUST IN CASE THEY HAVE SOMETHING IMPORTANT
	dma->__ctl_l__ |=
			//DMA_CTL_L_LLP_SRC_EN |
							//Block chaining is enabled on the source side only if the LLP_SRC_EN
							//field is high and LLPx.LOC is non-zero
			//DMA_CTL_L_LLP_DST_EN |
							//Block chaining is enabled on the destination side only if the LLP_DST_EN
							//field is high and LLPx.LOC is non-zero.
			DMA_CTL_L_SMS(0x0) |
							// Identifies the Master Interface layer from which the
							//source device (peripheral or memory) is accessed.
							//• 0h = MASTER_1
							//• 1h = MASTER_2
							//• 2h = MASTER_3
							//• 3h = MASTER_4
							//• 4h = NO_HARDCODE
			DMA_CTL_L_DMS(0x0) |
							//Identifies the Master Interface layer where the
							//destination device (peripheral or memory) resides.
							//• 0h = MASTER_1
							//• 1h = MASTER_2
							//• 2h = MASTER_3
							//• 3h = MASTER_4
							//• 4h = NO_HARDCODE
			DMA_CTL_L_TT_FC(0x3) |
							//• 0h = DMA
							//• 1h = SRC
							//• 2h = DST
							//• 3h = ANY
			//DMA_CTL_L_DST_SCATTER_EN |
							//• 0 = disabled
							//• 1 = enabled
			//DMA_CTL_L_SRC_GATHER_EN |
							//• 0 = disabled
							//• 1 = enabled
			DMA_CTL_L_SRC_MSIZE(0x4) |
							//Source Burst Transaction Length (SRC_MSIZE): Number of data items, each of
							//width CTLx.SRC_TR_WIDTH, to be read from the source every time a source burst
							//transaction request is made from either the corresponding hardware or software
							//handshaking interface.

							// NO INFO ABOUT VALUES
			DMA_CTL_L_DEST_MSIZE(0x4) |
							//Number of data items, each
							//of width CTLx.DST_TR_WIDTH, to be written to the destination every time a destination
							//burst transaction request is made from either the corresponding hardware or software
							//handshaking interface.

							// NO INFO ABOUT VALUES
			DMA_CTL_L_SINC(0x10) |
							//Indicates whether to increment or
							//decrement the source address on every source transfer.
							//• 00 = Increment
							//• 01 = Decrement
							//• 1x = No change
			DMA_CTL_L_DINC(0X10) |
							//Indicates whether to increment or
							//decrement the destination address on every destination transfer.
							//• 00 = Increment
							//• 01 = Decrement
							//• 1x = No change
			DMA_CTL_L_SRC_TR_WIDTH(0x0) |
							//This value must be less than or equal to
							//DMAH_Mx_HDATA_WIDTH, where x is the AHB layer 1 to 4 where the source resides
							//• 0h = 32 bits
							//• 1h = 64 bits
							//• 2h = 128 bits
							//• 3h = 256 bits
			DMA_CTL_L_DST_TR_WIDTH(0x0) |
							//This value must be less than or equal to
							//DMAH_Mk_HDATA_WIDTH, where k is the AHB layer 1 to 4 where the
							//destination resides.
							//• 0h = 32 bits
							//• 1h = 64 bits
							//• 2h = 128 bits
							//• 3h = 256 bits
			DMA_CTL_L_INT_EN;
							// Interrupts enable
	dma->__ctl_h__ &= DMA_CTL_H_RESERVED_MASK ;    //SAVE RESERVED VALUES JUST IN CASE THEY HAVE SOMETHING IMPORTANT
	dma->__ctl_h__ |=
			(~DMA_CTL_H_DONE) &
			 	 	 	 	//The LLI CTLx.DONE bit should be cleared when the linked lists are set up in memory, prior to
							//enabling the channel.
							//LLI accesses are always 32-bit accesses (Hsize = 2) aligned to 32-bit boundaries and
							//cannot be changed or programmed to anything other than 32-bit.
			DMA_CTL_H_BLOCK_TS(0x3) ;
							//When the DW_ahb_dmac is the flow controller, the
							//user writes this field before the channel is enabled in order to indicate the block size.
	dma->__cfg_l__ &= DMA_CFG_L_RESERVED_MASK ;
	dma->__cfg_l__ |=
			//DMA_CFG_L_RELOAD_DST |
							//The DARx register can be
							//automatically reloaded from its initial value at the end of every block for multi-block
							//transfers.
			//DMA_CFG_L_RELOAD_SRC |
							//The SARx register can be automatically
							//reloaded from its initial value at the end of every block for multi-block transfers.
			//DMA_CFG_L_MAX_ABRST(0X0) |
							//Maximum AMBA burst length that is
							//used for DMA transfers on this channel. A value of 0 indicates that software is not
							//limiting the maximum AMBA burst length for DMA transfers on this channel
			//DMA_CFG_L_SRC_HS_POL |
							//• 0 = Active high
							//• 1 = Active low
			//DMA_CFG_L_DST_HS_POL |
							//• 0 = Active high
							//• 1 = Active low
			//DMA_CFG_L_LOCK_B |
							//When active, the AHB bus master signal hlock is asserted for
							//the duration specified in CFGx.LOCK_B_L.
			//DMA_CFG_L_LOCK_CH |
							//When the channel is granted control of the master bus
							//interface and if the CFGx.LOCK_CH bit is asserted, then no other channels are granted
							//control of the master bus interface for the duration specified in CFGx.LOCK_CH_L.
							//Indicates to the master bus interface arbiter that this channel wants exclusive access to
							//the master bus interface for the duration specified in CFGx.LOCK_CH_L.
			//DMA_CFG_L_LOCK_B_L(0x2) |
							//Indicates the duration over which CFGx.LOCK_B bit
							//applies.
							//• 00 = Over complete DMA transfer
							//• 01 = Over complete DMA block transfer
							//• 1x = Over complete DMA transaction
			//DMA_CFG_L_LOCK_CH_L(0x2) |
							//Indicates the duration over which CFGx.LOCK_CH
							//bit applies.
							//• 00 = DMA transfer
							//• 01 = DMA block transfer
							//• 1x = DMA transaction
			DMA_CFG_L_HS_SEL_SRC |
							//Source Software or Hardware Handshaking Select
							//• 0 = HW handshaking
							//• 1 = SW handshaking
			DMA_CFG_L_HS_SEL_DST |
							//Source Software or Hardware Handshaking Select
							//• 0 = HW handshaking
							//• 1 = SW handshaking
			DMA_CFG_L_FIFO_EMPTY |
							// Indicates whether there is data left in the channel FIFO
			//DMA_CFG_L_CH_SUSP |
							// Suspends all DMA transfers from source until this bit is
							//cleared
			DMA_CFG_L_CH_PRIOR(0x7) ;
							// Priority of 7 is the highest priority.
	dma->__cfg_h__ &= DMA_CFG_H_RESERVED_MASK ;
	dma->__cfg_h__ |=
			//DMA_CFG_H_DEST_PER(0x0) |
							//Assigns a hardware handshaking interface (0 - DMAH_NUM_HS_INT-1) to
							//the destination of channel x if the CFGx.HS_SEL_DST field is 0; otherwise, this field is
							//ignored.
			//DMA_CFG_H_SRC_PER(0x0) |
			 	 	 	 	 //Assigns a hardware handshaking interface (0 - DMAH_NUM_HS_INT-1) to
							 //the source of channel x if the CFGx.HS_SEL_SRC field is 0; otherwise, this field is
							//ignored.
			//DMA_CFG_H_SS_UPD_EN |
							//Source status information is fetched
							// only from the location pointed to by the SSTATARx register, stored in the SSTATx
							// register and written out to the SSTATx location of the LLI (refer to Figure 52 on page
							// 243) if SS_UPD_EN is high.
			//DMA_CFG_H_DS_UPD_EN |
							// Destination status information is
							// fetched only from the location pointed to by the DSTATARx register, stored in the
							// DSTATx register and written out to the DSTATx location of the LLI if DS_UPD_EN is high.
			DMA_CFG_H_PROTCTL(0x1) ;
							//Protection Control bits used to drive the AHB HPROT[3:1] bus.
			//DMA_CFG_H_FIFO_MODE |
							//Determines how much space or data needs to be
							//available in the FIFO before a burst transaction request is serviced.
							//• 0 = Space/data available for single AHB transfer of the specified transfer width.
							//• 1 = Data available is greater than or equal to half the FIFO depth for destination
							//transfers and space available is greater than half the fifo depth for source transfers
			//DMA_CFG_H_FCMODE ;
							//Determines when source transaction requests are
							//serviced when the Destination Peripheral is the flow controller.
							//• 0 = Source transaction requests are serviced when they occur. Data pre-fetching is
							//enabled.
							//• 1 = Source transaction requests are not serviced until a destination transaction
							//request occurs.

	dma->__sgr_l__ = DMA_SGR_L_SGI(0x4);
	dma->__dsr_l__ = DMA_DSR_L_DSI(0x4);
	return 0;
}

int ws281x_spi_setup(ws281x_t *ws281x){
	volatile ssp_control_t *ssp_control = ws281x->devices->ssp_control_block;
	volatile ssp_general_t *ssp_general = ws281x->devices->ssp_general_block;

	ssp_control->__sscr0__ =
			SPI_SSP_SSCR0_MOD_NORMALSSPMODE |
			SPI_SSP_SSCR0_ACS_CLOCKBYAUDIOCLOCK |
			SPI_SSP_SSCR0_TIM_NOINTERRUPTFIFOUNDERRUN |
			SPI_SSP_SSCR0_RIM_NOINTERRUPTFIFOOVERRUN |
			SPI_SSP_SSCR0_NCS_CLOCKBYECS |
			SPI_SSP_SSCR0_EDSS_ONEPREPENDTODSS |
			SPI_SSP_SSCR0_SCR_SERIALCLOCKRATE(1024) |
			SPI_SSP_SSCR0_SSE_SSPDISABLE |
			SPI_SSP_SSCR0_ECS_CLOCKFROMONCHIPCLOCK |
			SPI_SSP_SSCR0_FRF_TEXASINSTRUMENTS |
			SPI_SSP_SSCR0_DSS_DATASIZESELECT(0xF) ;

	ssp_control->__sscr1__ =
			SPI_SSP_SSCR1_TTELP_TXDTRISTATEONCLOCKEDGE |
			SPI_SSP_SSCR1_TTE_TXDNOTTRISTATED |
			SPI_SSP_SSCR1_EBCEI_NOINTERRUPTONBITCOUNTERR |
			SPI_SSP_SSCR1_SCFR_SLAVECLKFREERUNNING |
			SPI_SSP_SSCR1_ECRA_NOREQCLOCKFROMOTHERSSP |
			SPI_SSP_SSCR1_ECRB_NOREQCLOCKFROMOTHERSSP |
			SPI_SSP_SSCR1_SCLKDIR_MASTERMODECLOCK |
			SPI_SSP_SSCR1_SFRMDIR_MASTERMODEFRAME |
			SPI_SSP_SSCR1_RWOT_TRANSMITANDRECEIVE |
			SPI_SSP_SSCR1_TRAIL_CPUHANDLETRAILINGBYTE | 	// NEED TO CHANGE
			SPI_SSP_SSCR1_TSRE_DMATRANSMITREQDISABLE | 		// NEED TO CHANGE
			SPI_SSP_SSCR1_RSRE_DMARECEIVEREQDISABLE | 		// NEED TO CHANGE
			SPI_SSP_SSCR1_TINTE_NOINTERRUPTONTIMEOUT |
			SPI_SSP_SSCR1_PINTE_NOTRAILINGBYTEINTERRUPT |
			SPI_SSP_SSCR1_IFS_FRAMEPOLARITYBYTHEFORMAT |
			SPI_SSP_SSCR1_RFT_RECEIVEFIFOTRIGTHRESHOLD(0x0) |
			SPI_SSP_SSCR1_TFT_TRANSMITFIFOTRIGTHRESHOLD(0x0) |
			SPI_SSP_SSCR1_TIE_TRANSFIFOLEVELINTERRDISA |  	// POSSIBLY CHANGE
			SPI_SSP_SSCR1_RIE_RECEIFIFOLEVELINTERRDISA ; 	// POSSIBLY CHANGE

	ssp_control->__ssdr__ = 0xACACACAC ;

	ssp_control->__ssacd__ =
			SPI_SSP_SSACD_ACPS_AUDIOCLK_14_857MHZ |
			SPI_SSP_SSACD_SCDB_SYSCLKDIVIDEVBY4 |
			SPI_SSP_SSACD_ACDS_AUDIOCLKDIVIDER(5) ;

	ssp_control->__sitf__ &= ~0xFFFF ;
	ssp_control->__sitf__ |=
			SPI_SSP_SITF_SETTRANSLOWWATERMARKSPI(0x0) |
			SPI_SSP_SITF_SETTRANSHIGHWATERMARKSPI(0xFF) ;
	ssp_general->__prv_clock_params__ =
			SPI_SSP_PRVCLKPARAMS_CLOCKUPDATE |
			SPI_SSP_PRVCLKPARAMS_N_DIVISOR(0xFF) |
			SPI_SSP_PRVCLKPARAMS_M_DIVIDEND(0x2) |
			SPI_SSP_PRVCLKPARAMS_ENABLECLOCK ;

}

int ws281x_init(ws281x_t *ws281x) {
	/*
	 *
	 * THIS IS FOR PWM. ABOUT TO DELETE.
	 *
	 */
	u_int8_t gpio_pin_number = ws281x->gpio_pin_number;
	if ((gpio_pin_number != 22) && (gpio_pin_number != 24)) {
		perror("GPIO PIN SPECIFIED IS NOT SUPPORTED.");
		return -1;
	}

	u_int8_t dma_ch_number = ws281x->dma_ch_number;
	if ((dma_ch_number < 0) || (dma_ch_number > 7)) {
		perror("DMA CHANNEL SPECIFIED DOES NOT EXIST.\nCHOOSE BETWEEN 0 & 7.");
		return -1;
	}

	ws281x->devices = malloc(sizeof(*ws281x->devices));

	volatile ws281x_devices_t *devices;
	devices = ws281x->devices;

	volatile u_int32_t *gpio_base;
	if (!(gpio_base = (volatile u_int32_t *) MAP_DEVICE(GPIO_SCORE_BASE_ADDR, BLOCK_SIZE))) {
		perror("GPIO BLOCK MAPPING FAILED!");
		return -1;
	}

	volatile u_int32_t *dma_base;
	if (!(dma_base = (volatile u_int32_t *) MAP_DEVICE(DMA_BASE_ADDR, BLOCK_SIZE))) {
		perror("DMA BLOCK MAPPING FAILED!");
		return -1;
	}

	volatile u_int32_t *spi_base;
	if (!(spi_base = (volatile u_int32_t *) MAP_DEVICE(SPI_BAR,	BLOCK_SIZE))) {
		perror("SPI BLOCK MAPPING FAILED!");
		return -1;
	}

	//Map GPIO & DMA pointer structures above the user space mapped previously.
	devices->gpio_pin_spi_mosi = (volatile gpio_t *) (gpio_base + gpio_pins[9]);
	devices->gpio_pin_spi_clk = (volatile gpio_t *) (gpio_base + gpio_pins[11]);
	devices->dma_ch = (volatile dma_channel_t *) (dma_base + dma_channels[dma_ch_number]);
	devices->dma_cfg = (volatile dma_cfg_t *) (dma_base + DMA_DMACCFG_OFF);
	devices->ssp_control_block = (volatile ssp_control_t *) spi_base;
	devices->ssp_general_block = (volatile ssp_general_t *) (spi_base + SPI_SSP_GENERAL_OFFSET);

	/*
	 *
	 * THIS IS FOR PWM. ABOUT TO DELETE.
	 *
	 */
	//PWM controllers registers are of u_int32_t size. Is not necessary to map an entire block for them.
	devices->pwm_dev = (volatile pwm_t *) MAP_DEVICE(gpio_pin_number == 22 ? PWM0_BASE_ADDR : PWM1_BASE_ADDR,sizeof(pwm_t));




	ws281x_fifo_init(ws281x);
	/*
	 *
	 * THIS IS FOR PWM. ABOUT TO DELETE. NEED TO BACKUP, LIGHT BULB DIMMER CONTROLS
	 *

	GPIO_CFG_FUNCTION(devices->gpio_pin->__cfg__,1)

	while (1){
	for (int i = 0x10 ; i < 0xff ; i++ ){
		devices->pwm_dev->__pwmctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_ON_TIME_DIVISOR(i) | 	PWM_BASE_UNIT(0x0002);
		usleep(10000);
	}
	for (int i = 0xff ; i > 0x10 ; i-- ){
		devices->pwm_dev->__pwmctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_ON_TIME_DIVISOR(i) | PWM_BASE_UNIT(0x0002);
		usleep(10000);
	}
	}
	*/


	ws281x_print_registers(ws281x);

	ws281x_spi_setup(ws281x);

	ws281x_print_registers(ws281x);

	/*
	ws281x_dma_setup(ws281x);

	//ws281x->devices->dma_cfg->__chenreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0xC3) | DMA_DMACHENREG_L_CH_EN(0xC3);
	ws281x->devices->dma_cfg->__chenreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x3C) | DMA_DMACHENREG_L_CH_EN(0x3C);
		//ws281x->devices->dma_cfg->__reqsrcreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x1) | DMA_DMACHENREG_L_CH_EN(0x1);
	//ws281x->devices->dma_cfg->__reqdstreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x1) | DMA_DMACHENREG_L_CH_EN(0x1);
	//ws281x->devices->dma_cfg->__lstsrcreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x1) | DMA_DMACHENREG_L_CH_EN(0x1);
	//ws281x->devices->dma_cfg->__lstdstreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x1) | DMA_DMACHENREG_L_CH_EN(0x1);
	usleep(10);
	//ws281x->devices->dma_cfg->__reqdstreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x3C) | DMA_DMACHENREG_L_CH_EN(0x3C);
	ws281x->devices->dma_cfg->__sglrqdstreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x3C) | DMA_DMACHENREG_L_CH_EN(0x3C);
	usleep(10);
	//ws281x->devices->dma_cfg->__reqsrcreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x3C) | DMA_DMACHENREG_L_CH_EN(0x3C);
	ws281x->devices->dma_cfg->__sglrqsrcreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x3C) | DMA_DMACHENREG_L_CH_EN(0x3C);

//	ws281x->devices->dma_cfg->__chenreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x00) | DMA_DMACHENREG_L_CH_EN(0x00);	ws281x->devices->dma_cfg->__sglrqdstreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x00) | DMA_DMACHENREG_L_CH_EN(0x00);	ws281x->devices->dma_cfg->__sglrqsrcreg_l__ = DMA_DMACHENREG_L_CH_EN_WE(0x00) | DMA_DMACHENREG_L_CH_EN(0x00);

	usleep(1000);

	ws281x_print_registers(ws281x);
	*/
	//ws281x_print_fifo(ws281x);

	return 0;
}
