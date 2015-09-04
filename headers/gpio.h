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
#define GPIO_OFF_PIN5			(0x118	/4)
#define GPIO_OFF_PIN6			(0x18	/4)
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

struct _pin_gpio_{
		char* 		__pin_gpio_name__;
		char* 		__pin_gpio_functions__;
		u_int32_t 	__pin_gpio_base__;
		u_int32_t 	__pin_gpio_offset__;
} gpio_pins_t[]={
		{"","",0x0},
		{"Pin 1",	"GND",	0x0},
		{"Pin 2",	"GND",	0x0},
		{"Pin 3",	"",		0x0},
		{"Pin 4",	"",		0x0},
		{"Pin 5",	"SPI_CS#/GPIO66",		GPIO_BASE_SCORE,	GPIO_OFF_PIN5},
		{"Pin 6",	"UART1_TXD/GPIO71",		GPIO_BASE_SCORE,	GPIO_OFF_PIN6},
		{"Pin 7",	"SPI_MISO/GPIO67",		GPIO_BASE_SCORE,	GPIO_OFF_PIN7},
		{"Pin 8",	"UART1_RXD/GPIO70",		GPIO_BASE_SCORE,	GPIO_OFF_PIN8},
		{"Pin 9",	"SPI_MOSI/GPIO68",		GPIO_BASE_SCORE,	GPIO_OFF_PIN9},
		{"Pin 10",	"UART1_CTS#/GPIO73",	GPIO_BASE_SCORE,	GPIO_OFF_PIN10},
		{"Pin 11",	"SPI_CLK/GPIO69",		GPIO_BASE_SCORE,	GPIO_OFF_PIN11},
		{"Pin 12",	"UART1_RTS#/GPIO72",	GPIO_BASE_SCORE,	GPIO_OFF_PIN12},
		{"Pin 13",	"I2C5_CLK/GPIO89",		GPIO_BASE_SCORE,	GPIO_OFF_PIN13},
		{"Pin 14",	"I2S2_CLK/GPIO62",		GPIO_BASE_SCORE,	GPIO_OFF_PIN14},
		{"Pin 15",	"I2C5_DATA/GPIO88",		GPIO_BASE_SCORE,	GPIO_OFF_PIN15},
		{"Pin 16",	"I2S2_FRM/GPIO63",		GPIO_BASE_SCORE,	GPIO_OFF_PIN16},
		{"Pin 17",	"UART2_TXD/GPIO75",		GPIO_BASE_SCORE,	GPIO_OFF_PIN17},
		{"Pin 18",	"I2S2_DATAOUT/GPIO65",	GPIO_BASE_SCORE,	GPIO_OFF_PIN18},
		{"Pin 19",	"UART2_RXD/GPIO74",		GPIO_BASE_SCORE,	GPIO_OFF_PIN19},
		{"Pin 20",	"I2S2_DATAIN/GPIO64",	GPIO_BASE_SCORE,	GPIO_OFF_PIN20},
		{"Pin 21",	"GPIO_S5_00",			GPIO_BASE_SSUS,		GPIO_OFF_PIN21},
		{"Pin 22",	"PWM[0]/GPIO94",		GPIO_BASE_SCORE,	GPIO_OFF_PIN22},
		{"Pin 23",	"GPIO_S5_01",			GPIO_BASE_SSUS,		GPIO_OFF_PIN23},
		{"Pin 24",	"PWM[1]/GPIO95",		GPIO_BASE_SCORE,	GPIO_OFF_PIN24},
		{"Pin 25",	"GPIO_S5_02",			GPIO_BASE_SSUS,		GPIO_OFF_PIN25},
		{"Pin 26",	"ILB_8254_SPKR/GPIO54",	GPIO_BASE_SCORE,	GPIO_OFF_PIN26},
		{NULL,NULL,NULL}
};


typedef struct{
		unsigned long GPIO_address_pointer = GPIO_BASE_SCORE;
		int GPIO_memory_file;
		void *GPIO_memory_map;
		volatile unsigned int *GPIO_memory_address;
} _gpio_;


/*****			GPIO MACROS			*****/



/*****			GPIO FUNCTIONS			*****/

_gpio_ INIT_GPIO(int pin);
u_int32_t GET_GPIO_VALUE();
u_int32_t SET_GPIO_VALUE();
u_int32_t GET_GPIO_FUNC();
u_int32_t SET_GPIO_FUNC();









