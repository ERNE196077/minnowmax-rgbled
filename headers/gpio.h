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

#define GPIO_INPUT				(0X1 << 1)
#define GPIO_OUTPUT				(0X0 << 1)
#define GPIO_VALUE(value)		(GPIO_OUTPUT | (value << 0))
#define GPIO_VALUE_SET			(GPIO_OUTPUT | (1 << 0))
#define GPIO_VALUE_CLR			GPIO_OUTPUT
#define GPIO_FUNCTION(func) 	(func << 0)




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
	u_int32_t __resv_0x067C__[610];
} __attribute__ ((packed)) gpio_t;


/*****			GPIO FUNCTIONS			*****/

int 	INIT_GPIO			(int pin_n);
int 	NEW_INIT_GPIO		();
void 	NEW_CLOSE_GPIO		();

void 	CLOSE_GPIO			(int pin_n);
void 	GET_GPIO_STATUS		(int pin_n);

int 	SET_GPIO_VALUE		(int pin_n);
int 	CLEAR_GPIO_VALUE	(int pin_n);

int		SET_GPIO_FUNC		(int pin_n, u_int8_t value);

int		SET_GPIO_DIR_INP	(int pin_n);
int		SET_GPIO_DIR_OUT	(int pin_n);

void	GET_REG_VALUE		(u_int32_t address);




