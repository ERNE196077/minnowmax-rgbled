/*
 * gpio.c
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

#include "headers/gpio.h"

/*****			GPIO VARIABLES			*****/

_pin_gpio_ *pin_gpio_t[26];

_info_gpio_ gpio_pins_t[28]={
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


/*****			GPIO FUNCTIONS			*****/

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

int CLOSE_GPIO(int pin_n){
	munmap(pin_gpio_t[pin_n]->__pin_gpio_memory_map_,BLOCK_SIZE);
	return 0;
}

void GET_GPIO_VALUE(int pin_n){
	printf("%04x\n",*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__));
	printf("%04x\n",*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2));
}


int 	GET_GPIO_DIR	(int pin_n){
	return 0;
}


int		SET_GPIO_DIR_INP	(int pin_n){
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) |= (2<<0);
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(5<<0);
	return 0;
}


int		SET_GPIO_DIR_OUT	(int pin_n){					//OUTPUT
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) |= (4<<0);
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(3<<0);
	return 0;
}


int 	SET_GPIO_VALUE	(int pin_n){
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) |= (1<<0);
	return 0;
}


int 	CLEAR_GPIO_VALUE	(int pin_n){
	*(pin_gpio_t[pin_n]->__pin_gpio_memory_address+pin_gpio_t[pin_n]->__pin_gpio_offset__+2) &= ~(1<<0);
	return 0;
}




