/*
 * gpio.h
 *
 * Creating the headers of the structs that will be used to connect the
 * application with the GPIO pins.
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

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/*****			GPIO BASE REGISTERS			*****/

#define BLOCK_SIZE			(4*1024)
#define	GPIO_BASE_SCORE 	0xFED0C000
#define	GPIO_BASE_SSUS		0xFED0E000

/*****			GPIO PIN REGISTERS			*****/

//#define GPIO_PIN1			GND
//#define GPIO_PIN2			GND
//#define GPIO_PIN3			+5v
//#define GPIO_PIN4			+3.3v
#define GPIO_PIN5			(GPIO_BASE_SCORE + 0x110	)
#define GPIO_PIN6			(GPIO_BASE_SCORE + 0x18		)
#define GPIO_PIN7			(GPIO_BASE_SCORE + 0x120	)
#define GPIO_PIN8			(GPIO_BASE_SCORE + 0x20		)
#define GPIO_PIN9			(GPIO_BASE_SCORE + 0x130	)
#define GPIO_PIN10			(GPIO_BASE_SCORE + 0x40		)
#define GPIO_PIN11			(GPIO_BASE_SCORE + 0x100	)
#define GPIO_PIN12			(GPIO_BASE_SCORE + 0x0		)
#define GPIO_PIN13			(GPIO_BASE_SCORE + 0x140	)
#define GPIO_PIN14			(GPIO_BASE_SCORE + 0xD0		)
#define GPIO_PIN15			(GPIO_BASE_SCORE + 0x150	)
#define GPIO_PIN16			(GPIO_BASE_SCORE + 0xC0		)
#define GPIO_PIN17			(GPIO_BASE_SCORE + 0x70		)
#define GPIO_PIN18			(GPIO_BASE_SCORE + 0xF0		)
#define GPIO_PIN19			(GPIO_BASE_SCORE + 0x60		)
#define GPIO_PIN20			(GPIO_BASE_SCORE + 0xE0		)
#define GPIO_PIN21			(GPIO_BASE_SSUS  + 0x1D0	)
#define GPIO_PIN22			(GPIO_BASE_SCORE + 0xA0		)
#define GPIO_PIN23			(GPIO_BASE_SSUS  + 0x210	)
#define GPIO_PIN24			(GPIO_BASE_SCORE + 0xB0		)
#define GPIO_PIN25			(GPIO_BASE_SSUS  + 0x1E0	)
#define GPIO_PIN26			(GPIO_BASE_SCORE + 0x670	)

struct _pin_{
		char* 		__pin_name__;
		char* 		__pin_functions__;
		u_int32_t 	__pin_gpio_address__;
} gpio_pins[]={
		{"","",0x0},
		{"Pin 1","",0x0},
		{"Pin 2","",0x0},
		{"Pin 3","",0x0},
		{"Pin 4","",0x0},
		{"Pin 5","",GPIO_PIN5},
		{"Pin 6","",GPIO_PIN6},
		{"Pin 7","",GPIO_PIN7},
		{"Pin 8","",GPIO_PIN8},
		{"Pin 9","",GPIO_PIN9},
		{"Pin 10","",GPIO_PIN10},
		{"Pin 11","",GPIO_PIN11},
		{"Pin 12","",GPIO_PIN12},
		{"Pin 13","",GPIO_PIN13},
		{"Pin 14","",GPIO_PIN14},
		{"Pin 15","",GPIO_PIN15},
		{"Pin 16","",GPIO_PIN16},
		{"Pin 17","",GPIO_PIN17},
		{"Pin 18","",GPIO_PIN18},
		{"Pin 19","",GPIO_PIN19},
		{"Pin 20","",GPIO_PIN20},
		{"Pin 21","",GPIO_PIN21},
		{"Pin 22","",GPIO_PIN22},
		{"Pin 23","",GPIO_PIN23},
		{"Pin 24","",GPIO_PIN24},
		{"Pin 25","",GPIO_PIN25},
		{"Pin 26","",GPIO_PIN26},
		{NULL,NULL,NULL}
};


/*****			GPIO MACROS			*****/



unsigned long GPIO_address_pointer = GPIO_BASE_SCORE;
int GPIO_memory_file;
void *GPIO_memory_map;
volatile unsigned int *GPIO_memory_address;






