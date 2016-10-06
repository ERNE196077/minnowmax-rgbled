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



/*****			GPIO BASE REGISTERS			*****/

#define	GPIO_SCORE_BASE_ADDR	 	0xFED0C000				// ALL GPIO's WITH THE EXCEPTION OF 21, 23 & 25
#define GPIO_SCORE_BASE_SIZE		0x67C
#define	GPIO_SUS_BASE_ADDR			0xFED0E000				// GPIO's 21, 23 & 25 Registers start in 0xFED0E1D0 using a page size multiple.
#define GPIO_SUS_BASE_SIZE			0x21C
#define GPIO_OFFSET_PIN5			(0x110	/4)
#define GPIO_OFFSET_PIN6			(0x10	/4)
#define GPIO_OFFSET_PIN7			(0x120	/4)
#define GPIO_OFFSET_PIN8			(0x20	/4)
#define GPIO_OFFSET_PIN9			(0x130	/4)
#define GPIO_OFFSET_PIN10			(0x40	/4)
#define GPIO_OFFSET_PIN11			(0x100	/4)
#define GPIO_OFFSET_PIN12			(0x0	/4)
#define GPIO_OFFSET_PIN13			(0x140	/4)
#define GPIO_OFFSET_PIN14			(0xD0	/4)
#define GPIO_OFFSET_PIN15			(0x150	/4)
#define GPIO_OFFSET_PIN16			(0xC0	/4)
#define GPIO_OFFSET_PIN17			(0x70	/4)
#define GPIO_OFFSET_PIN18			(0xF0	/4)
#define GPIO_OFFSET_PIN19			(0x60	/4)
#define GPIO_OFFSET_PIN20			(0xE0	/4)
#define GPIO_OFFSET_PIN21			(0x1D0	/4)
#define GPIO_OFFSET_PIN22			(0xA0	/4)
#define GPIO_OFFSET_PIN23			(0x210	/4)
#define GPIO_OFFSET_PIN24			(0xB0	/4)
#define GPIO_OFFSET_PIN25			(0x1E0	/4)
#define GPIO_OFFSET_PIN26			(0x670	/4)

uint32_t gpio_pins[] = {
		0x0,0x0,0x0,0x0,0x0,
		GPIO_OFFSET_PIN5,
		GPIO_OFFSET_PIN6,
		GPIO_OFFSET_PIN7,
		GPIO_OFFSET_PIN8,
		GPIO_OFFSET_PIN9,
		GPIO_OFFSET_PIN10,
		GPIO_OFFSET_PIN11,
		GPIO_OFFSET_PIN12,
		GPIO_OFFSET_PIN13,
		GPIO_OFFSET_PIN14,
		GPIO_OFFSET_PIN15,
		GPIO_OFFSET_PIN16,
		GPIO_OFFSET_PIN17,
		GPIO_OFFSET_PIN18,
		GPIO_OFFSET_PIN19,
		GPIO_OFFSET_PIN20,
		GPIO_OFFSET_PIN21,
		GPIO_OFFSET_PIN22,
		GPIO_OFFSET_PIN23,
		GPIO_OFFSET_PIN24,
		GPIO_OFFSET_PIN25,
		GPIO_OFFSET_PIN26
};

/*****			GPIO MACROS			*****/
#define GPIO_VAL_INPUT(val_reg)				val_reg |= (0X1 << 1);
#define GPIO_VAL_OUTPUT(val_reg)			val_reg &= ~(0X1 << 1);
#define GPIO_VAL_SET(val_reg)				val_reg |= 0x1;
#define GPIO_VAL_CLR(val_reg)				val_reg &= ~0x1;
#define GPIO_VAL_INPUT_GET(val_reg)			(val_reg & 0x1);
#define GPIO_CFG_FUNCTION(cfg_reg,func)		cfg_reg &= ~0x7; cfg_reg |= func;


/*****			GPIO STRUCTURES			*****/
typedef struct gpio_t{
	uint32_t __cfg__;
	uint32_t __resv__;
	uint32_t __val__;
	
}__attribute__ ((packed)) gpio_t;

typedef struct{
    volatile __u32	*gpio_base;
	volatile gpio_t *gpio_pin_spi_mosi;
    volatile gpio_t *gpio_pin_spi_clk;
} gpio_dev_t;

#endif /* GPIO_H_ */