
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/pci.h>
#include "headers/dma.h"


