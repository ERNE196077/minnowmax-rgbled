
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
#include <linux/pci.h>


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




