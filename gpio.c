/*
 * gpio.c
 *
 * GPIO Functionality.
 * When a GPIO device is read (INIT_GPIO), it's mapping is stored in the respective number of the "pin_gpio_t" table. 
 * "gpio_pins_t" stores only the main information of the GPIO's (Functions, address, offset).
 *
 *
 *  Created on: Sep 3, 2015
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

#include "headers/gpio.h"

/*****			GPIO VARIABLES			*****/

_pin_gpio_ *pin_gpio_t[26];

_info_gpio_ gpio_pins_t[28]={
		{"","",0x0},
		{"Pin 1",	"GND",			"",				0x0},
		{"Pin 2",	"GND",			"",				0x0},
		{"Pin 3",	"",				"",				0x0},
		{"Pin 4",	"",				"",				0x0},
		{"Pin 5",	"GPIO66",		"SPI_CS#",		GPIO_BASE_SCORE,	GPIO_OFF_PIN5},
		{"Pin 6",	"GPIO71",		"UART1_TXD",	GPIO_BASE_SCORE,	GPIO_OFF_PIN6},
		{"Pin 7",	"GPIO67",		"SPI_MISO",		GPIO_BASE_SCORE,	GPIO_OFF_PIN7},
		{"Pin 8",	"GPIO70",		"UART1_RXD",	GPIO_BASE_SCORE,	GPIO_OFF_PIN8},
		{"Pin 9",	"GPIO68",		"SPI_MOSI",		GPIO_BASE_SCORE,	GPIO_OFF_PIN9},
		{"Pin 10",	"GPIO73",		"UART1_CTS#",	GPIO_BASE_SCORE,	GPIO_OFF_PIN10},
		{"Pin 11",	"GPIO69",		"SPI_CLK",		GPIO_BASE_SCORE,	GPIO_OFF_PIN11},
		{"Pin 12",	"GPIO72",		"UART1_RTS#",	GPIO_BASE_SCORE,	GPIO_OFF_PIN12},
		{"Pin 13",	"GPIO89",		"I2C5_CLK",		GPIO_BASE_SCORE,	GPIO_OFF_PIN13},
		{"Pin 14",	"GPIO62",		"I2S2_CLK",		GPIO_BASE_SCORE,	GPIO_OFF_PIN14},
		{"Pin 15",	"GPIO88",		"I2C5_DATA",	GPIO_BASE_SCORE,	GPIO_OFF_PIN15},
		{"Pin 16",	"GPIO63",		"I2S2_FRM",		GPIO_BASE_SCORE,	GPIO_OFF_PIN16},
		{"Pin 17",	"GPIO75",		"UART2_TXD",	GPIO_BASE_SCORE,	GPIO_OFF_PIN17},
		{"Pin 18",	"GPIO65",		"I2S2_DATAOUT",	GPIO_BASE_SCORE,	GPIO_OFF_PIN18},
		{"Pin 19",	"GPIO74",		"UART2_RXD",	GPIO_BASE_SCORE,	GPIO_OFF_PIN19},
		{"Pin 20",	"GPIO64",		"I2S2_DATAIN",	GPIO_BASE_SCORE,	GPIO_OFF_PIN20},
		{"Pin 21",	"GPIO_S5_00",	"N/A",			GPIO_BASE_SSUS,		GPIO_OFF_PIN21},
		{"Pin 22",	"GPIO94",		"PWM[0]",		GPIO_BASE_SCORE,	GPIO_OFF_PIN22},
		{"Pin 23",	"GPIO_S5_01",	"N/A",			GPIO_BASE_SSUS,		GPIO_OFF_PIN23},
		{"Pin 24",	"GPIO95",		"PWM[1]",		GPIO_BASE_SCORE,	GPIO_OFF_PIN24},
		{"Pin 25",	"GPIO_S5_02",	"N/A",			GPIO_BASE_SSUS,		GPIO_OFF_PIN25},
		{"Pin 26",	"GPIO54",		"ILB_8254_SPKR",GPIO_BASE_SCORE,	GPIO_OFF_PIN26},
		{NULL,NULL,NULL}
};


/*
Inits the GPIO reading, is needed to run CLOSE_GPIO to unmap the GPIO device. Returns 0 if successful and -1 in error. **NEED TO IMPROVE**

@param	pin_n 	Number of the pin to map and read.

@returns 	0 on successful mapping/reading, -1 on error.

*/
int INIT_GPIO(int pin_n){

	pin_gpio_t[pin_n]= (_pin_gpio_*)malloc(sizeof(_pin_gpio_));
	pin_gpio_t[pin_n]->__pin_gpio_offset__ = gpio_pins_t[pin_n].__pin_gpio_offset__;
	pin_gpio_t[pin_n]->__pin_gpio_base_addr_ptr__ = gpio_pins_t[pin_n].__pin_gpio_base_addr_ptr__;


	if (( pin_gpio_t[pin_n]->__pin_gpio_memory_file__ = open("/dev/mem",O_RDWR|O_SYNC)) < 0 ){
			printf("Chingo a su madre, problema de permisos. Resultado: %d\n",pin_gpio_t[pin_n]->__pin_gpio_memory_file__);
			return -1;
		}

		pin_gpio_t[pin_n]->__pin_gpio_memory_map_ = mmap(
				NULL,
				BLOCK_SIZE,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				pin_gpio_t[pin_n]->__pin_gpio_memory_file__,
				pin_gpio_t[pin_n]->__pin_gpio_base_addr_ptr__
				);

		if ( pin_gpio_t[pin_n]->__pin_gpio_memory_map_ == MAP_FAILED ) {
		        perror("Memory Mapping Failed");
		        return -1;
		   }
		pin_gpio_t[pin_n]->__pin_gpio_memory_address = (volatile unsigned int*)pin_gpio_t[pin_n]->__pin_gpio_memory_map_;
		close(pin_gpio_t[pin_n]->__pin_gpio_memory_file__);


	return 0;
}


/*
Unmap the GPIO device and release the memory. **NEED TO IMPROVE**

@param	pin_n 	Number of the pin to map and read.

@returns 	n/a

*/
void CLOSE_GPIO(int pin_n){
	munmap(pin_gpio_t[pin_n]->__pin_gpio_memory_map_,BLOCK_SIZE);
}


/*
Gather the GPIO pin status, which function is active, the direction and register values for manual debugging **VALIDATE AGAINST __pin_gpio_memory_map__**

@param	pin_n 	Number of the pin to map and read.

@returns 	n/a

*/
void GET_GPIO_STATUS(int pin_n){
	if (!pin_gpio_t[pin_n])
		printf("Status:\t\tNot reading\n");
	else{
		printf("STATUS:\t\tReading\n");

		printf("Functions:\t");
		int8_t func=*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__) & 0x7;
			printf("%s",gpio_pins_t[pin_n].__pin_gpio_function0__);	if ( func==0 )	printf(" <<ACTIVE>> \n");	else	printf("\n");
			printf("%s",gpio_pins_t[pin_n].__pin_gpio_function1__);	if ( func==1 )	printf(" <<ACTIVE>> \n");	else	printf("\n");

		printf("Direction:\t");
		int8_t inp=(*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) & 0x4) >> 2;
		int8_t out=(*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) & 0x2) >> 1;
		if ( (inp == 0) && (out==1) )
			printf("In\n");
		else{
			if( ((inp==1) && (out==0)) || ((inp==0) && (out==0)) )
				printf("Out\n");
			else{
				printf("None\n");

			}
		}
		printf("Value:\t\t%i\n\n",*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) & 0x1);
		printf("Register Values:\n");
		printf("%02x : %04x\n",pin_gpio_t[pin_n]->__pin_gpio_offset__*4,*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__));
		printf("%02x : %04x%04x\n",pin_gpio_t[pin_n]->__pin_gpio_offset__*4+0x8,*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+1),*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2));
	}
}


/*
Set GPIO as input.	**NEED TO VERIFY IF GPIO IS MAPPED FIRST**

@param	pin_n 	Number of the pin to map and read.

@returns 	0 when successful.

*/
int		SET_GPIO_DIR_INP	(int pin_n){
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) |= (1<<1);
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(5<<0);
	return 0;
}


/*
Set GPIO as output.	**NEED TO VERIFY IF GPIO IS MAPPED FIRST**

@param	pin_n 	Number of the pin to map and read.

@returns 	0 when successful.

*/
int		SET_GPIO_DIR_OUT	(int pin_n){					//OUTPUT
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(1<<2);
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(3<<0);
	return 0;
}


/*
Set GPIO as as high.	**NEED TO VERIFY IF GPIO IS MAPPED FIRST AND AS OUTPUT**

@param	pin_n 	Number of the pin to map and read.

@returns 	0 when successful.

*/
int 	SET_GPIO_VALUE	(int pin_n){
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) |= (1<<0);
	return 0;
}


/*
Set GPIO as as low.	**NEED TO VERIFY IF GPIO IS MAPPED FIRST AND AS OUTPUT**

@param	pin_n 	Number of the pin to map and read.

@returns 	0 when successful.

*/
int 	CLEAR_GPIO_VALUE	(int pin_n){
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(1<<0);
	return 0;
}


/*
Switch between GPIO or the alternative function

@param	pin_n 	Number of the pin to map and read.
@param	func 	Number of the function. Usually 0 is GPIO and 1 is the alternative function.

@returns 	0 when successful, -1 when a not supported function is choosen.

*/
int		SET_GPIO_FUNC		(int pin_n, u_int8_t func){
	if ( func > 1 )
		return -1;
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__) &= ~7;
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__) |= func;
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~7;
	return 0;
}


