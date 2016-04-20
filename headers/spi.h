#ifndef SPI_H_
#define SPI_H_
#endif /* DMA_H_ */



#define SPI_BAR 			0x90c18000
#define SPI_SSP_GENERAL_OFFSET	(0x400 / 4)



typedef struct{
u_int32_t __sscr0__;
u_int32_t __sscr1__;
u_int32_t __sssr__;
u_int32_t __ssitr__;
u_int32_t __ssdr__;
u_int32_t __rsv_0x014__[5];
u_int32_t __ssto__;
u_int32_t __sspsp__;
u_int32_t __sstsa__;
u_int32_t __ssrsa__;
u_int32_t __sstss__;
u_int32_t __ssacd__;
u_int32_t __itf__;
u_int32_t __sitf__;
u_int32_t __sirf__;
}__attribute__((packed)) ssp_control;

typedef struct{
u_int32_t __prv_clock_params__;
u_int32_t __resets__;
u_int32_t __general__;
u_int32_t __ssp_reg__;
u_int32_t __rsv_0x410__[2];
u_int32_t __spi_cs_ctrl__;
}__attribute__((packed)) ssp_general;
