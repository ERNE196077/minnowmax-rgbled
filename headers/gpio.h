/*
 * gpio.h
 *
 * Creating the headers of the structs that will be used to connect the
 * application with the GPIO pins.
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

#define	GPIO_BASE		0xFED0C000
#define GPIO_OFFSET		0x00
#define BLOCK_SIZE		(4*1024)

unsigned long GPIO_address_pointer = GPIO_BASE;
int GPIO_memory_file;
void *GPIO_memory_map;
volatile unsigned int *GPIO_memory_address;






