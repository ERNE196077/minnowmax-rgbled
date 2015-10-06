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

#define BLOCK_SIZE			(4*1024)
#define	GPIO_BASE_SCORE 	0xFED0C000
#define	GPIO_BASE_SSUS		0xFED0E000


/*****			GPIO PIN REGISTER OFFSETS			*****/

//#define GPIO_PIN1			GND
//#define GPIO_PIN2			GND
//#define GPIO_PIN3			+5v
//#define GPIO_PIN4			+3.3v
#define GPIO_OFF_PIN5			(0x110	/4)
#define GPIO_OFF_PIN6			(0x10	/4)
#define GPIO_OFF_PIN7			(0x120	/4)
#define GPIO_OFF_PIN8			(0x20	/4)
#define GPIO_OFF_PIN9			(0x130	/4)
#define GPIO_OFF_PIN10			(0x40	/4)
#define GPIO_OFF_PIN11			(0x100	/4)
#define GPIO_OFF_PIN12			(0x0	/4)
#define GPIO_OFF_PIN13			(0x140	/4)
#define GPIO_OFF_PIN14			(0xD0	/4)
#define GPIO_OFF_PIN15			(0x150	/4)
#define GPIO_OFF_PIN16			(0xC0	/4)
#define GPIO_OFF_PIN17			(0x70	/4)
#define GPIO_OFF_PIN18			(0xF0	/4)
#define GPIO_OFF_PIN19			(0x60	/4)
#define GPIO_OFF_PIN20			(0xE0	/4)
#define GPIO_OFF_PIN21			(0x1D0	/4)
#define GPIO_OFF_PIN22			(0xA0	/4)
#define GPIO_OFF_PIN23			(0x210	/4)
#define GPIO_OFF_PIN24			(0xB0	/4)
#define GPIO_OFF_PIN25			(0x1E0	/4)
#define GPIO_OFF_PIN26			(0x670	/4)


/*****			GPIO STRUCTURES			*****/

typedef struct {
		char* 					__pin_gpio_name__;
		char* 					__pin_gpio_function0__;
		char* 					__pin_gpio_function1__;
		unsigned long 			__pin_gpio_base_addr_ptr__;
		u_int32_t 				__pin_gpio_offset__;
		int 					__pin_gpio_memory_file__;
		void 					*__pin_gpio_memory_map_;
		volatile unsigned int 	*__pin_gpio_memory_address;


}_pin_gpio_;

typedef struct {
		char* 					__pin_gpio_name__;
		char* 					__pin_gpio_function0__;
		char* 					__pin_gpio_function1__;
		unsigned long 			__pin_gpio_base_addr_ptr__;
		u_int32_t 				__pin_gpio_offset__;


}_info_gpio_;


/*****			GPIO FUNCTIONS			*****/

int 	INIT_GPIO			(int pin_n);
void 	CLOSE_GPIO			(int pin_n);
void 	GET_GPIO_STATUS		(int pin_n);

int 	SET_GPIO_VALUE		(int pin_n);
int 	CLEAR_GPIO_VALUE	(int pin_n);

int		SET_GPIO_FUNC		(int pin_n, u_int8_t value);

int		SET_GPIO_DIR_INP	(int pin_n);
int		SET_GPIO_DIR_OUT	(int pin_n);






