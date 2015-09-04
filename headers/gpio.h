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

#define BLOCK_SIZE			(4*1024)
#define	GPIO_BASE_SCORE 	0xFED0C000
#define	GPIO_BASE_SSUS		0xFED0E000

//#define GPIO_PIN1			GND
//#define GPIO_PIN2			GND
//#define GPIO_PIN3			+5v
//#define GPIO_PIN4			+3.3v
#define GPIO_PIN5			(GPIO_BASE_SCORE + 272	)	// 0xFED0C000 + 110h
#define GPIO_PIN6			(GPIO_BASE_SCORE + 24	)	// 0xFED0C000 + 18h
#define GPIO_PIN7			(GPIO_BASE_SCORE + 288	)	// 0xFED0C000 + 120h
#define GPIO_PIN8			(GPIO_BASE_SCORE + 32	)	// 0xFED0C000 + 20h
#define GPIO_PIN9			(GPIO_BASE_SCORE + 304	)	// 0xFED0C000 + 130h
#define GPIO_PIN10			(GPIO_BASE_SCORE + 64	)	// 0xFED0C000 + 40h
#define GPIO_PIN11			(GPIO_BASE_SCORE + 256	)	// 0xFED0C000 + 100h
#define GPIO_PIN12			(GPIO_BASE_SCORE + 0	)	// 0xFED0C000 + 0h
#define GPIO_PIN13			(GPIO_BASE_SCORE + 320	)	// 0xFED0C000 + 140h
#define GPIO_PIN14			(GPIO_BASE_SCORE + 208	)	// 0xFED0C000 + D0h
#define GPIO_PIN15			(GPIO_BASE_SCORE + 336	)	// 0xFED0C000 + 150h
#define GPIO_PIN16			(GPIO_BASE_SCORE + 192	)	// 0xFED0C000 + C0h
#define GPIO_PIN17			(GPIO_BASE_SCORE + 70	)	// 0xFED0C000 + 70h
#define GPIO_PIN18			(GPIO_BASE_SCORE + 240	)	// 0xFED0C000 + F0h
#define GPIO_PIN19			(GPIO_BASE_SCORE + 96	)	// 0xFED0C000 + 60h
#define GPIO_PIN20			(GPIO_BASE_SCORE + 224	)	// 0xFED0C000 + E0h
#define GPIO_PIN21			(GPIO_BASE_SSUS  + 464	)	// 0xFED0E000 + 1D0h
#define GPIO_PIN22			(GPIO_BASE_SCORE + 160	)	// 0xFED0C000 + A0h
#define GPIO_PIN23			(GPIO_BASE_SSUS  + 528	)	// 0xFED0E000 + 210h
#define GPIO_PIN24			(GPIO_BASE_SCORE + 176	)	// 0xFED0C000 + B0h
#define GPIO_PIN25			(GPIO_BASE_SSUS  + 480	)	// 0xFED0E000 + 1E0
#define GPIO_PIN26			(GPIO_BASE_SCORE + 1648	)	// 0xFED0C000 + 670h


unsigned long GPIO_address_pointer = GPIO_BASE_SCORE;
int GPIO_memory_file;
void *GPIO_memory_map;
volatile unsigned int *GPIO_memory_address;






