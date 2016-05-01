

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "headers/spi.h"

void print_spi_status (u_int32_t spi_stat){
	printf("\n#####\tSPI STATUS\t#####\n\n");
	printf("Bit Count Error : \t\t%s\n",spi_stat & SPI_SSP_SSSR_BCE_BITCOUNTERROR ? "ERROR: SSPSFRM signal has been asserted when the bit counter was not 0" : "NO ERRORS" );
	printf("Clock Sync Status : \t\t%s\n",spi_stat  & SPI_SSP_SSSR_CSS_CLOCKSYNCSTATUS  ? "SSP is currently busy synchronizing slave mode signals " : "SSP is ready for slave clock operations " );
	printf("Transmit FIFO Underun : \t%s\n",spi_stat  & SPI_SSP_SSSR_TUR_TRANSMITFIFOUNDERRUN ? "Attempted read from the transmit FIFO when the FIFO was empty, requestinterrupt" : "NO ERRORS" );
	printf("End of Chain : \t\t\t%s\n",spi_stat  & SPI_SSP_SSSR_EOC_ENDOFDMACHAIN ? "DMA has signaled an end of chain condition" : "DMA has not signaled an end of chain condition " );
	printf("Receiver TimeOut Interr : \t%s\n",spi_stat  & SPI_SSP_SSSR_TINT_RECEIVERTIMEOUTINTERRUPT ? "Receiver time-out pending" : "No receiver time-out pending " );
	printf("Trailling Byte Interr : \t%s\n",spi_stat  & SPI_SSP_SSSR_PINT_PENDINGTRAILINGBYTE ? "Peripheral trailing byte interrupt pending" : "No peripheral trailing byte interrupt pending" );
	printf("Receive FIFO Level : \t\t%i\n",(spi_stat  & SPI_SSP_SSSR_RFL_RECEIVEFIFOLEVEL) >> 12 );
	printf("Transmit FIFO Level : \t\t%i\n", (spi_stat  & SPI_SSP_SSSR_TFL_TRANSMITFIFOLEVEL) >> 8 );
	printf("Receive FIFO Overrun : \t\t%s\n",spi_stat  & SPI_SSP_SSSR_ROR_RECEIVEFIFOOVERRUN ? "Attempted data write to full receive FIFO, request interrupt" : "Receive FIFO has not experienced an overrun" );
	printf("Receive FIFO Service Req : \t%s\n",spi_stat  & SPI_SSP_SSSR_RFS_RECEIVEFIFOSERVICEREQ ? "Receive FIFO level exceeds RFT threshold (RFT), request interrupt" : "Receive FIFO level is at or below RFT threshold (RFT), or SSP disabled" );
	printf("Transmit FIFO Service Req : \t%s\n",spi_stat  & SPI_SSP_SSSR_TFS_TRANSMITFIFOSERVICEREQ ? "Transmit FIFO level is at or below TFT threshold (TFT+1), request interrupt" : "Transmit FIFO level exceeds the TFT threshold (TFT+1), or SSP disabled" );
	printf("SSP Busy : \t\t\t%s\n",spi_stat  & SPI_SSP_SSSR_BSY_SPIBUSY ? "SSP currently transmitting or receiving a frame" : "SSP is idle or disabled " );
	printf("Receive FIFO Not Empty : \t%s\n",spi_stat  & SPI_SSP_SSSR_RNE_RECEIVEFIFONOTEMPTY ? "Receive FIFO is not empty" : "Receive FIFO is empty" );
	printf("Transmit FIFO Not Full : \t%s\n",spi_stat  & SPI_SSP_SSSR_TNF_TRANSMITFIFONOTFULL ? "Transmit FIFO is not full" : "Transmit FIFO is full" );
}
