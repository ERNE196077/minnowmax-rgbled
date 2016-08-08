

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "headers/spi.h"

struct spi_dev_t{
	volatile __u32          *spi_base;
	volatile ssp_control_t  *ssp_control_block;
    volatile ssp_general_t  *ssp_general_block;
    
};