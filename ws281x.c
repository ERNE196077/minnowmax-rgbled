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
#include <sys/ioctl.h>
#include "ws281x_dma.h"

			//DELETE AFTER FINISH LIB
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
	volatile dma_channel_t *dma_ch_2;
	volatile dma_channel_t *dma_ch_3;
	volatile dma_channel_t *dma_ch_4;
		volatile dma_cfg_t *dma_cfg;
	volatile gpio_t *gpio_pin_spi_mosi;
	volatile gpio_t *gpio_pin_spi_clk;
	volatile u_int32_t *fifo_data;
	volatile u_int32_t fifo_data2[11];
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

	u_int32_t offset = ((intptr_t) addr /  BLOCK_SIZE) * 8 ;			// Formule to get the offset inside the pagemap.
	u_int32_t reminder = (intptr_t)addr % BLOCK_SIZE;					// Reminder of the virtual address, will be added to the physical addres to point the memory.


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
	//printf("BEFORE STRIP:\t%" PRIx64 "\n", page_frame_buffer);

	page_frame_buffer &= 0x7fffffffffffff; 									// We are only interested in the first 54 bits.
	page_frame_buffer <<= 12;												// Need to move bits left 12 times to get the correct boundary.
	page_frame_buffer += reminder;											// We add the reminder offset.
	//printf("AFTER STRIP:\t%" PRIx64 "\n", page_frame_buffer);
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
	printf("__dar_h__\t\t%08x\n", dma->__rsv_0x0c__);
	printf("__llp_l__\t\t%08x\n", dma->__llp_l__);
	printf("__llp_h__\t\t%08x\n", dma->__rsv_0x14__);
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
	printf("Reserved 0\t\t%08x\n", ssp_ctrl->__rsv_0x014__[0]);
	printf("Reserved 1\t\t%08x\n", ssp_ctrl->__rsv_0x014__[1]);
	printf("Reserved 2\t\t%08x\n", ssp_ctrl->__rsv_0x014__[2]);
	printf("Reserved 3\t\t%08x\n", ssp_ctrl->__rsv_0x014__[3]);
	printf("Reserved 4\t\t%08x\n", ssp_ctrl->__rsv_0x014__[4]);
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

	print_spi_status(ssp_ctrl->__sssr__);
	print_dma_status(dma_cfg->__dmaparamsch0__);

}

void ws281x_print_fifo(ws281x_t *ws281x) {
	volatile u_int32_t *word = &((volatile u_int32_t *) ws281x->devices->fifo_data)[0];
	volatile u_int32_t *word2 = &((volatile u_int32_t *) ws281x->devices->fifo_data2)[0];

	printf("%p\n", ws281x->devices->fifo_data);
	printf("%lx\n", (intptr_t)word);

	printf("FIFO 1 \t\tFIFO 2\n");

	for (int i = 0; i < WORDCOUNT(ws281x->lednumber); i++)
		printf("%08x\t%08x\n", *(word + (i)), *(word2 + (i)));
}

int ws281x_fifo_init(ws281x_t *ws281x) {
	led_t *ledarray = ws281x->ledarray;

	//Allocate memory for the led color data
	ws281x->devices->fifo_data = malloc(BYTECOUNT(ws281x->lednumber));

	ws281x->devices->fifo_data2[0] = 0x009249B6;
		ws281x->devices->fifo_data2[1] = 0x00DB6DA4;
		ws281x->devices->fifo_data2[2] = 0x00936DB6;
		ws281x->devices->fifo_data2[3] = 0x00DB6DA4;
		ws281x->devices->fifo_data2[4] = 0x009249B6;
		ws281x->devices->fifo_data2[5] = 0x00DB6DA4;
		ws281x->devices->fifo_data2[6] = 0x00936DB6;
		ws281x->devices->fifo_data2[7] = 0x00DB6DA4;

		ws281x->devices->fifo_data2[8] = 0x009249B6;
		/*
		 * 	ws281x->devices->fifo_data2[0] = 0x9249B6DB;
	ws281x->devices->fifo_data2[1] = 0x6DA4936D;
	ws281x->devices->fifo_data2[2] = 0xB69249B6;
	ws281x->devices->fifo_data2[3] = 0xDB6DA400;
	ws281x->devices->fifo_data2[4] = 0xDB6DA400;
	ws281x->devices->fifo_data2[5] = 0xDB6DA400;
	ws281x->devices->fifo_data2[6] = 0xDB6DA400;
	ws281x->devices->fifo_data2[7] = 0xDB6DA400;
	ws281x->devices->fifo_data2[8] = 0xDB6DA400;
	ws281x->devices->fifo_data2[9] = 0xDB6DA400;
	ws281x->devices->fifo_data2[10] = 0xDB6DA400;
*/
	if (!ws281x->devices->fifo_data)
		return -1;

	volatile u_int32_t *word = &((volatile u_int32_t *) ws281x->devices->fifo_data)[0];
	//memset(word, 0, BYTECOUNT(ws281x->lednumber));

	for (int i = 0; i < ws281x->lednumber; i++) {
		int j = 24 * i;
		*(word + (j++)) = 0x9249B6DB;//((*ledarray).r & 1) ? WS281X_PWM_REG_HIGH : WS281X_PWM_REG_LOW;
		*(word + (j++)) = 0x6DA4936D;
		*(word + (j++)) = 0xB69249B6;
		*(word + (j++)) = 0xDB6DA400;
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
	volatile dma_cfg_t *dma_cfg = ws281x->devices->dma_cfg;

	volatile dma_channel_t *dma2 = ws281x->devices->dma_ch_2;
	u_int32_t addr2 = (intptr_t)ws281x_convert_virtual(&ws281x->devices->dma_ch_2);
	printf("\n\n\n\n%08x\n",addr2);
	printf("%p\n",(intptr_t)addr2);
	printf("%p\n",(intptr_t)(addr2 << 2));
	printf("%p\n\n\n\n",(intptr_t)(addr2 >> 29));



	volatile dma_channel_t *dma3 = ws281x->devices->dma_ch_3;

	u_int32_t addr3 = (intptr_t)ws281x_convert_virtual(&ws281x->devices->dma_ch_3);
		printf("\n\n\n\n%08x\n",addr3);
		printf("%p\n\n\n\n",(intptr_t)&ws281x->devices->dma_ch_3);

	dma_cfg->__dmacfgre_l__ = 0x1;

	dma->__sar_l__ = ws281x_convert_virtual(&ws281x->devices->fifo_data2[0]);

	dma->__dar_l__ = SPI_BAR+0x10;//(u_int32_t)0x90825000;

	dma->__ctl_l__ =
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
	dma->__llp_l__ = DMA_LLP_LO_ADDRESSOFNEXTLLP((intptr_t)addr2);
	dma->__rsv_0x14__ = (intptr_t)(addr2 >> 29);

	dma->__ctl_h__ =
			DMA_CTL_HI_DONE_DONEBITZERO |
			DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(1) ;

	dma->__cfg_l__ =
			DMA_CFG_LO_RELOADDST_NORELOADDSTAFTERTRANSFER |
			DMA_CFG_LO_RELOADSRC_NORELOADSRCAFTERTRANSFER |
			DMA_CFG_LO_MAXABRST_NOLIMITBURSTTRANSFER |
			DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVEHIGH |
			DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVEHIGH |
			DMA_CFG_LO_LOCKB_BUSNOTLOCKED |
			DMA_CFG_LO_LOCKCH_CHANNELNOTLOCKED |
			DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSFER |
			DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSFER |
			DMA_CFG_LO_HSSELSRC_SOURCESWHANDSHAKING |				// TO BE CHANGED PROBABLY
			DMA_CFG_LO_HSSELDST_DESTSWHANDSHAKING |
			DMA_CFG_LO_FIFOEMPTY_SETVALUEZERO |
			DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
			DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY ;


	dma->__cfg_h__ =
			DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(0x0) |
			DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
			DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
			DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
			DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
			DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

	dma->__sgr_l__ = DMA_SGR_LO_SGI_SRCGATHERINCDECMULTIPLE(64);

	dma->__dsr_l__ = DMA_DSR_LO_DSI_DESTSCATTERINCDECMULTIPLE(32);


	/////////////////////// Segundo

	dma2->__sar_l__ = ws281x_convert_virtual(&ws281x->devices->fifo_data2[1]);

	dma2->__dar_l__ = SPI_BAR+0x10;//(u_int32_t)0x90825000;

	dma2->__ctl_l__ =
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
	dma2->__llp_l__ = DMA_LLP_LO_ADDRESSOFNEXTLLP((intptr_t)addr3);
	dma2->__rsv_0x14__ = (intptr_t)(addr2 >> 29);

	dma2->__ctl_h__ =
			DMA_CTL_HI_DONE_DONEBITZERO |
			DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(1) ;

	dma2->__cfg_l__ =
			DMA_CFG_LO_RELOADDST_NORELOADDSTAFTERTRANSFER |
			DMA_CFG_LO_RELOADSRC_NORELOADSRCAFTERTRANSFER |
			DMA_CFG_LO_MAXABRST_NOLIMITBURSTTRANSFER |
			DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVEHIGH |
			DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVEHIGH |
			DMA_CFG_LO_LOCKB_BUSNOTLOCKED |
			DMA_CFG_LO_LOCKCH_CHANNELNOTLOCKED |
			DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSFER |
			DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSFER |
			DMA_CFG_LO_HSSELSRC_SOURCESWHANDSHAKING |				// TO BE CHANGED PROBABLY
			DMA_CFG_LO_HSSELDST_DESTSWHANDSHAKING |
			DMA_CFG_LO_FIFOEMPTY_SETVALUEZERO |
			DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
			DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY ;


	dma2->__cfg_h__ =
			DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(0x0) |
			DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
			DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
			DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
			DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
			DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

	dma2->__sgr_l__ = DMA_SGR_LO_SGI_SRCGATHERINCDECMULTIPLE(64);

	dma2->__dsr_l__ = DMA_DSR_LO_DSI_DESTSCATTERINCDECMULTIPLE(32);

	/////////////////////// Tercero

		dma3->__sar_l__ = ws281x_convert_virtual(&ws281x->devices->fifo_data2[2]);

		dma3->__dar_l__ = SPI_BAR+0x10;//(u_int32_t)0x90825000;

		dma3->__ctl_l__ =
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
		dma3->__llp_l__ = 0x0;
		dma3->__rsv_0x14__ = 0x0;
		dma3->__ctl_h__ =
				DMA_CTL_HI_DONE_DONEBITZERO |
				DMA_CTL_HI_BLOCKTS_DMAFLOWBLOCKSIZE(0) ;

		dma3->__cfg_l__ =
				DMA_CFG_LO_RELOADDST_NORELOADDSTAFTERTRANSFER |
				DMA_CFG_LO_RELOADSRC_NORELOADSRCAFTERTRANSFER |
				DMA_CFG_LO_MAXABRST_NOLIMITBURSTTRANSFER |
				DMA_CFG_LO_SRCHSPOL_SRCHANDSHAKEACTIVEHIGH |
				DMA_CFG_LO_DSTHSPOL_DSTHANDSHAKEACTIVEHIGH |
				DMA_CFG_LO_LOCKB_BUSNOTLOCKED |
				DMA_CFG_LO_LOCKCH_CHANNELNOTLOCKED |
				DMA_CFG_LO_LOCKBL_BUSLOCKEDOVERDMATRANSFER |
				DMA_CFG_LO_LOCKCHL_CHLOCKEDOVERDMATRANSFER |
				DMA_CFG_LO_HSSELSRC_SOURCESWHANDSHAKING |				// TO BE CHANGED PROBABLY
				DMA_CFG_LO_HSSELDST_DESTSWHANDSHAKING |
				DMA_CFG_LO_FIFOEMPTY_SETVALUEZERO |
				DMA_CFG_LO_CHSUSP_NOSUSREACTIVATEDMAACTIVITY |
				DMA_CFG_LO_CHPRIOR_HIGHESTPRIORITY ;


		dma3->__cfg_h__ =
				DMA_CFG_HI_DESTPER_DSTHWHANDSHAKEIFACE(0x0) |
				DMA_CFG_HI_SRCPER_SRCHWHANDSHAKEIFACE(0x0) |
				DMA_CFG_HI_SSUPDEN_DISABLESRCSTATUSUPDATE |
				DMA_CFG_HI_DSUPDEN_DISABLEDSTSTATUSUPDATE |
				DMA_CFG_HI_FIFOMODE_SPACEDATAEQTOTRANSWDITH |
				DMA_CFG_HI_FCMODE_SRCTRANSREQWHENTHEYOCURR ;

		dma3->__sgr_l__ = DMA_SGR_LO_SGI_SRCGATHERINCDECMULTIPLE(64);

		dma3->__dsr_l__ = DMA_DSR_LO_DSI_DESTSCATTERINCDECMULTIPLE(32);

	return 0;
}

int	ws281x_dma_start (ws281x_t *ws281x){
	volatile dma_cfg_t *dma_cfg = ws281x->devices->dma_cfg;
	u_int8_t ch_num = ws281x->dma_ch_number;

	dma_cfg->__chenreg_l__ = (0x1 << (8 + ch_num)) | (0x1 << ch_num);
	dma_cfg->__reqdstreg_l__ = (0x1 << (8 + ch_num)) | (0x1 << ch_num);
	dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + ch_num)) | (0x1 << ch_num);

	dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + ch_num)) | (0x1 << ch_num);
	dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + ch_num)) | (0x1 << ch_num);

	return 0;
}

int	ws281x_dma_stop (ws281x_t *ws281x){
	volatile dma_cfg_t *dma_cfg = ws281x->devices->dma_cfg;
	u_int8_t ch_num = ws281x->dma_ch_number;

	dma_cfg->__chenreg_l__ = (0x1 << (8 + ch_num));
	dma_cfg->__reqdstreg_l__ = (0x1 << (8 + ch_num));
	dma_cfg->__reqsrcreg_l__ = (0x1 << (8 + ch_num));

	dma_cfg->__sglrqdstreg_l__ = (0x1 << (8 + ch_num)) ;
	dma_cfg->__sglrqsrcreg_l__ = (0x1 << (8 + ch_num)) ;


	return 0;
}




int	ws281x_gpio_setup (ws281x_t *ws281x){
	volatile gpio_t *gpio_mosi = ws281x->devices->gpio_pin_spi_mosi;
	volatile gpio_t *gpio_clk = ws281x->devices->gpio_pin_spi_clk;

	GPIO_CFG_FUNCTION(gpio_mosi->__cfg__,1);
	GPIO_CFG_FUNCTION(gpio_clk->__cfg__,1);


	return 0;
}

int ws281x_spi_setup (ws281x_t *ws281x){
	volatile ssp_control_t *ssp_control = ws281x->devices->ssp_control_block;
	volatile ssp_general_t *ssp_general = ws281x->devices->ssp_general_block;

	ssp_control->__sscr0__ =
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

	//ssp_control->__ssdr__ = 0xACACACAC ;

	ssp_control->__ssacd__ =
			SPI_SSP_SSACD_ACPS_AUDIOCLK_32_842MHZ |
			SPI_SSP_SSACD_SCDB_SYSCLKNODIVIDED |
			SPI_SSP_SSACD_ACDS_AUDIOCLKDIVIDER(2) ;

	ssp_control->__sitf__ &= ~0xFFFF ;
	ssp_control->__sitf__ |=
			SPI_SSP_SITF_SETTRANSLOWWATERMARKSPI(0x0) |
			SPI_SSP_SITF_SETTRANSHIGHWATERMARKSPI(0xFF) ;
	ssp_general->__prv_clock_params__ =
			SPI_SSP_PRVCLKPARAMS_CLOCKUPDATE |
			SPI_SSP_PRVCLKPARAMS_N_DIVISOR(0xFF) |
			SPI_SSP_PRVCLKPARAMS_M_DIVIDEND(0x3) |
			SPI_SSP_PRVCLKPARAMS_ENABLECLOCK ;

	return 0;

}

int ws281x_spi_start (ws281x_t *ws281x){
	volatile ssp_control_t *ssp_control = ws281x->devices->ssp_control_block;

	ssp_control->__sscr0__ |= SPI_SSP_SSCR0_SSE_SSPENABLE;

	return 0;
}

int ws281x_spi_stop (ws281x_t *ws281x){
	volatile ssp_control_t *ssp_control = ws281x->devices->ssp_control_block;

	ssp_control->__sscr0__ &= ~SPI_SSP_SSCR0_SSE_SSPENABLE;

	return 0;
}

int ws281x_spi_additems (ws281x_t *ws281x){
	volatile ssp_control_t *ssp_control = ws281x->devices->ssp_control_block;
	//u_int32_t testnumber[2] = 0xACACACAC , 0xBCBCBCBC);
	//ssp_control->__ssdr__ = 0xDB6DB600;
	//ssp_control->__ssdr__ = 0xDB492400;
	ssp_control->__ssdr__ = 0x9249B6;
	ssp_control->__ssdr__ = 0xDB6DA4;
	ssp_control->__ssdr__ = 0x936DB6;
	ssp_control->__ssdr__ = 0x9249B6;
	/*
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
	ssp_control->__ssdr__ =  0xACACACAC;
*/

	return 0;
}

int ws281x_spi_getreceived (ws281x_t *ws281x){
	volatile ssp_control_t *ssp_control = ws281x->devices->ssp_control_block;

	u_int32_t gathered = 0x0;

	gathered |= ssp_control->__ssdr__ ;
	printf("\n%08x", gathered);

	gathered |= ssp_control->__ssdr__ ;
	printf("\n%08x", gathered);
/*
	gathered |= ssp_control->__ssdr__ ;
	printf("\n%08x", gathered);

	gathered |= ssp_control->__ssdr__ ;
	printf("\n%08x", gathered);

	gathered |= ssp_control->__ssdr__ ;
	printf("\n%08x\n\n", gathered);
*/
	return 0;
}

int ioctl_printdmamem(int file_desc, u_int32_t *dma_reg){
    ioctl(file_desc, IOCTL_PRINTDMAMEM, dma_reg);
    printf("DEFERENCED *dma_reg FROM KERNEL %08x\n", *dma_reg);
    printf("POINTER dma_reg FROM KERNEL %p\n", dma_reg);

    return 0;
}


int ioctl_reldmamem(int file_desc){
    ioctl(file_desc, IOCTL_RELDMAMEM);
    return 0;
}

int ioctl_reqdmamem(int file_desc, u_int32_t *dma_reg){
    ioctl(file_desc, IOCTL_REQDMAMEM, dma_reg);
    return 0;
}

int ws281x_init (ws281x_t *ws281x) {
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

    devices->dma_ch_2 = (volatile dma_channel_t *) aligned_alloc(32,sizeof(dma_channel_t));
    devices->dma_ch_3 = (volatile dma_channel_t *) aligned_alloc(32,sizeof(dma_channel_t));
    devices->dma_ch_4 = (volatile dma_channel_t *) aligned_alloc(32,sizeof(dma_channel_t));
	printf ( "%08x &devices->dma_ch_2\n", ws281x_convert_virtual(&devices->dma_ch_2 	));
	printf ( "%08x &devices->dma_ch_2\n", (intptr_t)&devices->dma_ch_2);
		printf ( "%08x devices->dma_ch_2\n\n", (intptr_t)devices->dma_ch_2);

		printf ( "%08x &devices->dma_ch_3\n", ws281x_convert_virtual(&devices->dma_ch_3 	));
		printf ( "%08x &devices->dma_ch_3\n", (intptr_t)&devices->dma_ch_3);
			printf ( "%08x devices->dma_ch_3\n\n", (intptr_t)devices->dma_ch_3);

			printf ( "%08x Convert &devices->dma_ch_4\n", ws281x_convert_virtual(&devices->dma_ch_4 	));
				printf ( "%08x &devices->dma_ch_4\n", (intptr_t)&devices->dma_ch_4);
					printf ( "%08x devices->dma_ch_4\n", (intptr_t)devices->dma_ch_4);
					printf ( "%08x Convert devicesLlego?->ch4\n\n", ws281x_convert_virtual(devices->dma_ch_4 	));


u_int32_t prueba = 4;

    int file = open("/dev/"DEV_NAME,0);
	if (file < 0) {
		printf("Can't open device file: %s\n", DEV_NAME);
		exit(-1);
	}

/*	ioctl_printdmamem(file, &prueba);
    printf("DEFERENCED prueba FROM KERNEL %08x\n", prueba);
    printf("POINTER &prueba FROM KERNEL %p\n", &prueba);

    prueba = 8;
    ioctl_reldmamem(file);
    printf("DEFERENCED prueba FROM KERNEL %08x\n", prueba);
    printf("POINTER &prueba FROM KERNEL %p\n", &prueba);
*/
    ioctl_reqdmamem(file,&prueba);
    printf("DEFERENCED prueba FROM KERNEL %08x\n", prueba);
    printf("POINTER &prueba FROM KERNEL %p\n", &prueba);

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
	//ws281x_spi_stop(ws281x);

	//ws281x_dma_stop(ws281x);
	//usleep(100);
	//ws281x_print_registers(ws281x);
	//ws281x_fifo_init(ws281x);

	//ws281x_gpio_setup(ws281x);

	//ws281x_spi_setup(ws281x);
	//ws281x_print_registers(ws281x);


	//ws281x_spi_setup(ws281x);
	//ws281x_spi_start(ws281x);


	//ws281x_dma_setup(ws281x);
	//ws281x_print_registers(ws281x);

	//ws281x_dma_start(ws281x);
	//ws281x_spi_additems(ws281x);

	//usleep(100);

	//ws281x_print_registers(ws281x);



	return 0;
}
