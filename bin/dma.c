
/*
 * dma.c
 *
 * DMA Functionality
 * Library designed to control the flow of the DMA, is needed to create blocks
 * of dma configuration pages and concatenate them as a double linked list to 
 * propagate the WS2812b functionality
 *
 *
 *  Created on: Oct 28, 2015
 *      Author: minnow
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "headers/dma.h"


u_int32_t dma_channels[] = {
		DMA_OFFSET_CH0,
		DMA_OFFSET_CH1,
		DMA_OFFSET_CH2,
		DMA_OFFSET_CH3,
		DMA_OFFSET_CH4,
		DMA_OFFSET_CH5,
		DMA_OFFSET_CH6,
		DMA_OFFSET_CH7
		};

int print_dma_status (u_int32_t channel_reg){
	char *fifo_depth[] = {"8 bits", "16 bits", "32 bits", "64 bits", "128 bits"};
	char *master_select[] = { "MASTER 1", "MASTER 2", "MASTER 3", "MASTER 4", "NO HARDCODE" };
	char *max_mult_size[] = {"4 bits", "8 bits", "16 bits", "32 bits", "64 bits", "128 bits", "256 bits", "Reserved"};
	char *fc[] = {"DMA", "SRC", "DST", "ANY"};
	char *tw[] = {"NO HARDCODE", "8 bits", "16 bits", "32 bits", "64 bits", "128 bits", "256 bits", "Reserved"};
	printf("FIFO Depth :\t\t%s\n", fifo_depth[(channel_reg & DMA_DMAPARAMSCH_FIFODEPTH) >> 28]);
	printf("SRC Master Select :\t%s\n", master_select[(channel_reg & DMA_DMAPARAMSCH_SMS) >> 25]);
	printf("LMS :\t\t\t%s\n", master_select[(channel_reg & DMA_DMAPARAMSCH_LMS) >> 22]);
	printf("DST Master Select :\t%s\n", master_select[(channel_reg & DMA_DMAPARAMSCH_DMS) >> 19]);
	printf("Max Multi Size : \t%s\n", max_mult_size[(channel_reg & DMA_DMAPARAMSCH_MAXMULTSIZE) >> 16]);
	printf("Flow Control by : \t%s\n", fc[(channel_reg & DMA_DMAPARAMSCH_FC) >> 14]);
	printf("Chainned LLPs :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_HCLLP) >> 13 == 1 ? "TRUE" : "FALSE" );
	printf("CTL WB EN :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_CTLWBEN) >> 12 == 1 ? "TRUE" : "FALSE" );
	printf("Multi Block Enable :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_MULTIBLKEN) >> 11 == 1 ? "TRUE" : "FALSE" );
	printf("Lock Enable :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_LOCKEN) >> 10 == 1 ? "TRUE" : "FALSE" );
	printf("SRC Gather :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_SRCGATEN) >> 9 == 1 ? "TRUE" : "FALSE" );
	printf("DST Scatter :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_DSTSCAEN) >> 8 == 1 ? "TRUE" : "FALSE" );
	printf("Retrieve SRC status :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_STATSRC) >> 7 == 1 ? "TRUE" : "FALSE" );
	printf("Retreive DST status :\t\t%s\n", (channel_reg & DMA_DMAPARAMSCH_STATDST) >> 6 == 1 ? "TRUE" : "FALSE" );
	printf("Source TW : \t%s\n", tw[(channel_reg & DMA_DMAPARAMSCH_STW) >> 3]);
	printf("Destination TW : \t%s\n", tw[(channel_reg & DMA_DMAPARAMSCH_STW)]);
	return 0;
}


