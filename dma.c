
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


/*_dma_lli_t_ llist = NULL;

void INIT_DMA_LIST(_dma_lli_t_ *head){
	void *add = head->address;
	memset(add,0,sizeof(add));
	head->next = head;
	head->prev = head;
}



_dma_lli_t_ *NEXT_DMA_ITEM (_dma_lli_t_ *head, _dma_lli_t_ *lli){
	_dma_lli_t_ *temp = head;


	return temp->next;
}

void PRINT_DMA_LIST(_dma_lli_t_ head){
	_dma_lli_t_ *temp = head;
	if ( (temp->next == head) && (temp->prev == head) )
	{
		printf("1: %08x\n", temp->address);
	}
}
*/
