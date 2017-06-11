#ifndef RGBLED_DRV_H_
#define RGBLED_DRV_H_

#include <linux/ioctl.h>

#define BLOCK_SIZE_T 4096
#define MAJOR_NUM 121
#define DEV_NAME "rgbled_device"

/********  RGBLED FUNCTIONS   ********/
#define IOCTL_RGBLED_SETCONFIG       	_IOW(MAJOR_NUM , 0 , __u32 )
#define IOCTL_RGBLED_USERLEDS          	_IO(MAJOR_NUM , 1 )
#define IOCTL_RGBLED_FUNCTION        	_IO(MAJOR_NUM , 2 )
#define IOCTL_RGBLED_RENDER             _IO(MAJOR_NUM , 3 )
#define IOCTL_RGBLED_USERCOLOR            	_IO(MAJOR_NUM , 4 )

#define RGBLED_CONF_TYPEWS281X			(0)
#define RGBLED_CONF_TYPEAPA102 			(1 << 28)
#define RGBLED_CONF_DMACH(value)		((value) << 24)
#define	RGBLED_CONF_LEDNUM(value)		(value)
#define RGBLED_CONF_ANIMATION(value)	((value) << 16)
#define RGBLED_CONF_APA102ENDFRAMES(lednum)	((lednum) / 64 ) 

#define RGBLED_CONF_GET_LEDNUM(value)		((value) & 0xFFFF)
#define RGBLED_CONF_GET_LEDTYPE(value)		(((value) & 0xF0000000) >> 28)
#define RGBLED_CONF_GET_DMACH(value)		(((value) & 0xF000000) >> 24)
#define RGBLED_CONF_GET_ANIMATION(value) 	(((value) & 0xFF0000) >> 16)


/*****			RGBLEDS STRUCTURES			*****/

/*		rgbled_conf_t ::  4 bit led type ; 4 bits DMA channel ; 8 bits Animation ;  16 bits led number 	*/
/*                        0 0 0 0          0 0 0 0              0 0 0 0  0 0 0 0    0000 0000 0000 0000 */
typedef uint32_t rgbled_conf_t;

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}__attribute__ ((packed)) led_t;

typedef enum{
	leds_off,
	leds_on,
	leds_userdefined,
	leds_warm,
	leds_solar,
	leds_green,
	leds_red,
	leds_blue,
	leds_function1,
	leds_function2,
	leds_function3
}led_function_t;

typedef enum{
	ledtype_ws281x,
	ledtype_apa102
}led_type_t;




#endif /* RGBLED_DRV_H_ */
