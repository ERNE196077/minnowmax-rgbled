
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


_dma_lli_ llist = NULL;

void INIT_DMA_LIST(_dma_lli_ *head){
	void *add = head->__address__;
	memset(add,0,sizeof(add));
	head->__next__ = head;
	head->__prev__ = head;
}



_dma_lli_ *NEXT_DMA_ITEM (_dma_lli_ *head, _dma_lli_ *lli){
	_dma_lli_ *temp = head;
	for ( temp != lli  ){
		temp = temp->__next__;
	}

	return temp->__next__;
}

void PRINT_DMA_LIST(_dma_lli_ head){
	_dma_lli_ *temp = head;
	if ( (temp->__next__ == head) && (temp->__prev__ == head) )
	{
		printf("1: %08x\n", temp->__address__);
	}
	for ( temp != lli  ){
		temp = temp->__next__;
	}
}