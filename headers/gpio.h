/*
 * gpio.h
 *
 * Structs and macros to control the GPIO functions. Basic operations:
 * - Change Function
 * - Init GPIO reading
 * - Close GPIO reading
 * - Set High
 * - Set Low
 *
 *
 * PIN	FUNCTION
 * 1	GND
 * 2	GND
 * 3	5 V
 * 4	3.3 V
 * 5	SIO_SPI_CS1
 * 6	UART1_TXD
 * 7	SIO_SPI_MISO
 * 8	UART1_RXD
 * 9	SIO_SPI_MOSI
 * 10	UART1_CTSB
 * 11	SIO_SPI_CLK
 * 12	UART1_RTSB
 * 13	GPIO_I2C_SCL
 * 14	I2SCLK_GPIO
 * 15	GPIO_I2C_SDA
 * 16	I2SFRM_GPIO
 * 17	UART2_TXD
 * 18	I2SDO_GPIO
 * 19	UART2_RXD
 * 20	I2SDI_GPIO
 * 21	GPIO_S5_0
 * 22	PWM0
 * 23	GPIO_S5_1
 * 24	PWM1
 * 25	GPIO_S5_2
 * 26	ILB_8254_GPIO
 *
 *
 *
 *  Created on: Jul 30, 2015
 *      Author: minnow
 */

#ifndef GPIO_H_
#define GPIO_H_
#endif /* GPIO_H_ */


/*****			GPIO BASE REGISTERS			*****/

#define	GPIO_SCORE_BASE_ADDRESS 	0xFED0C000				// ALL GPIO's WITH THE EXCEPTION OF 21, 23 & 25
#define	GPIO_SUS_BASE_ADDRESS		0xFED0E000				// GPIO's 21, 23 & 25 Registers start in 0xFED0E1D0 using a page size multiple.


/*****			GPIO MACROS			*****/

#define GPIO_VAL_INPUT(val_reg)				val_reg |= (0X1 << 1);
#define GPIO_VAL_OUTPUT(val_reg)			val_reg &= ~(0X1 << 1);
#define GPIO_VAL_SET(val_reg)				val_reg |= 0x1;
#define GPIO_VAL_CLR(val_reg)				val_reg &= ~0x1;
#define GPIO_VAL_INPUT_GET(val_reg)			(val_reg & 0x1);
#define GPIO_CFG_FUNCTION(cfg_reg,func)		cfg_reg &= ~0x7; cfg_reg |= func;


/*****			GPIO STRUCTURES			*****/

typedef struct{
	u_int32_t __gpio_12_cfg__;
	u_int32_t __resv_0x0004__;
	u_int32_t __gpio_12_val__;	
	u_int32_t __resv_0x000C__;
	u_int32_t __gpio_06_cfg__;
	u_int32_t __resv_0x0014__;
	u_int32_t __gpio_06_val__;	
	u_int32_t __resv_0x001C__;
	u_int32_t __gpio_08_cfg__;
	u_int32_t __resv_0x0024__;
	u_int32_t __gpio_08_val__;	
	u_int32_t __resv_0x002C__[5];
	u_int32_t __gpio_10_cfg__;
	u_int32_t __resv_0x0044__;
	u_int32_t __gpio_10_val__;	
	u_int32_t __resv_0x004C__[5];
	u_int32_t __gpio_19_cfg__;
	u_int32_t __resv_0x0064__;
	u_int32_t __gpio_19_val__;	
	u_int32_t __resv_0x006C__;
	u_int32_t __gpio_17_cfg__;
	u_int32_t __resv_0x0074__;
	u_int32_t __gpio_17_val__;	
	u_int32_t __resv_0x007C__[9];
	u_int32_t __gpio_22_cfg__;
	u_int32_t __resv_0x00A4__;
	u_int32_t __gpio_22_val__;	
	u_int32_t __resv_0x00AC__;
	u_int32_t __gpio_24_cfg__;
	u_int32_t __resv_0x00B4__;
	u_int32_t __gpio_24_val__;	
	u_int32_t __resv_0x00BC__;
	u_int32_t __gpio_16_cfg__;
	u_int32_t __resv_0x00C4__;
	u_int32_t __gpio_16_val__;	
	u_int32_t __resv_0x00CC__;
	u_int32_t __gpio_14_cfg__;
	u_int32_t __resv_0x00D4__;
	u_int32_t __gpio_14_val__;	
	u_int32_t __resv_0x00DC__;
	u_int32_t __gpio_20_cfg__;
	u_int32_t __resv_0x00E4__;
	u_int32_t __gpio_20_val__;	
	u_int32_t __resv_0x00EC__;
	u_int32_t __gpio_18_cfg__;
	u_int32_t __resv_0x00F4__;
	u_int32_t __gpio_18_val__;	
	u_int32_t __resv_0x00FC__;
	u_int32_t __gpio_11_cfg__;
	u_int32_t __resv_0x0104__;
	u_int32_t __gpio_11_val__;	
	u_int32_t __resv_0x010C__;
	u_int32_t __gpio_05_cfg__;
	u_int32_t __resv_0x0114__;
	u_int32_t __gpio_05_val__;	
	u_int32_t __resv_0x011C__;
	u_int32_t __gpio_07_cfg__;
	u_int32_t __resv_0x0124__;
	u_int32_t __gpio_07_val__;	
	u_int32_t __resv_0x012C__;
	u_int32_t __gpio_09_cfg__;
	u_int32_t __resv_0x0134__;
	u_int32_t __gpio_09_val__;	
	u_int32_t __resv_0x013C__;
	u_int32_t __gpio_13_cfg__;
	u_int32_t __resv_0x0144__;
	u_int32_t __gpio_13_val__;	
	u_int32_t __resv_0x014C__;
	u_int32_t __gpio_15_cfg__;
	u_int32_t __resv_0x0154__;
	u_int32_t __gpio_15_val__;	
	u_int32_t __resv_0x015C__[325];
	u_int32_t __gpio_26_cfg__;
	u_int32_t __resv_0x0674__;
	u_int32_t __gpio_26_val__;
} __attribute__ ((packed)) gpio_score_t;

typedef struct{
	u_int32_t __resv_0x0000__[116];
	u_int32_t __gpio_21_cfg__;	
	u_int32_t __resv_0x01D4__;
	u_int32_t __gpio_21_val__;
	u_int32_t __resv_0x01DC__;
	u_int32_t __gpio_25_cfg__;	
	u_int32_t __resv_0x01E4__;
	u_int32_t __gpio_25_val__;
	u_int32_t __resv_0x01EC__[9];
	u_int32_t __gpio_23_cfg__;	
	u_int32_t __resv_0x0214__;
	u_int32_t __gpio_23_val__;
} __attribute__ ((packed)) gpio_sus_t;

typedef struct{
	u_int32_t __gpio_cfg__;	
	u_int32_t __resv__;
	u_int32_t __gpio_val__;
	
}__attribute__ ((packed)) gpio_t;