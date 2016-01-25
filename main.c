#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include "headers/general.h"
#include "headers/gpio.h"
#include "headers/pwm.h"


int main (){

	printf("Hello World! Bicho\n");

	//volatile gpio_t *gpio_map = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS);
	//volatile gpio_score_t *gpio_score = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS, sizeof(gpio_score_t));

	volatile gpio_sus_t *gpio_sus = MAP_DEVICE(GPIO_SUS_BASE_ADDRESS, sizeof(gpio_sus_t));

//	GPIO_CFG_FUNCTION(gpio_score->__gpio_05_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_06_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_07_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_08_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_09_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_10_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_11_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_12_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_13_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_14_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_15_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_16_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_17_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_18_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_19_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_20_cfg__,0)
	GPIO_CFG_FUNCTION(gpio_sus->__gpio_21_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_22_cfg__,0)
	GPIO_CFG_FUNCTION(gpio_sus->__gpio_23_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_24_cfg__,0)
	GPIO_CFG_FUNCTION(gpio_sus->__gpio_25_cfg__,0)
//	GPIO_CFG_FUNCTION(gpio_score->__gpio_26_cfg__,0)

	printf("%08x\n", gpio_score->__gpio_05_cfg__);
	printf("%08x\n", gpio_score->__gpio_06_cfg__);
	printf("%08x\n", gpio_score->__gpio_07_cfg__);
	printf("%08x\n", gpio_score->__gpio_08_cfg__);
	printf("%08x\n", gpio_score->__gpio_09_cfg__);
	printf("%08x\n", gpio_score->__gpio_10_cfg__);
	printf("%08x\n", gpio_score->__gpio_11_cfg__);
	printf("%08x\n", gpio_score->__gpio_12_cfg__);
	printf("%08x\n", gpio_score->__gpio_13_cfg__);
	printf("%08x\n", gpio_score->__gpio_14_cfg__);
	printf("%08x\n", gpio_score->__gpio_15_cfg__);
	printf("%08x\n", gpio_score->__gpio_16_cfg__);
	printf("%08x\n", gpio_score->__gpio_17_cfg__);
	printf("%08x\n", gpio_score->__gpio_18_cfg__);
	printf("%08x\n", gpio_score->__gpio_19_cfg__);
	printf("%08x\n", gpio_score->__gpio_20_cfg__);
	printf("%08x\n", gpio_score->__gpio_22_cfg__);
	printf("%08x\n", gpio_score->__gpio_24_cfg__);
	printf("%08x\n\n", gpio_score->__gpio_26_cfg__);





//		GPIO_VAL_OUTPUT(gpio_score->__gpio_05_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_06_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_07_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_08_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_09_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_10_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_11_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_12_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_13_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_14_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_15_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_16_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_17_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_18_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_19_val__)
//		GPIO_VAL_OUTPUT(gpio_score->__gpio_20_val__)
		//GPIO_VAL_OUTPUT(gpio_sus->__gpio_21_val__)
		GPIO_VAL_OUTPUT(gpio_score->__gpio_22_val__)
		//GPIO_VAL_OUTPUT(gpio_sus->__gpio_23_val__)
		GPIO_VAL_OUTPUT(gpio_score->__gpio_24_val__)
		//GPIO_VAL_OUTPUT(gpio_sus->__gpio_25_val__)
		GPIO_VAL_OUTPUT(gpio_score->__gpio_26_val__)

GPIO_VAL_SET(gpio_score->__gpio_05_val__)
GPIO_VAL_SET(gpio_score->__gpio_06_val__)
GPIO_VAL_SET(gpio_score->__gpio_07_val__)
GPIO_VAL_SET(gpio_score->__gpio_08_val__)
GPIO_VAL_SET(gpio_score->__gpio_09_val__)
GPIO_VAL_SET(gpio_score->__gpio_10_val__)
GPIO_VAL_SET(gpio_score->__gpio_11_val__)
GPIO_VAL_SET(gpio_score->__gpio_12_val__)
GPIO_VAL_SET(gpio_score->__gpio_13_val__)
GPIO_VAL_SET(gpio_score->__gpio_14_val__)
GPIO_VAL_SET(gpio_score->__gpio_15_val__)
GPIO_VAL_SET(gpio_score->__gpio_16_val__)
GPIO_VAL_SET(gpio_score->__gpio_17_val__)
GPIO_VAL_SET(gpio_score->__gpio_18_val__)
GPIO_VAL_SET(gpio_score->__gpio_19_val__)
GPIO_VAL_SET(gpio_score->__gpio_20_val__)
//GPIO_VAL_SET(gpio_sus->__gpio_21_val__)
GPIO_VAL_SET(gpio_score->__gpio_22_val__)
//GPIO_VAL_SET(gpio_sus->__gpio_23_val__)
GPIO_VAL_SET(gpio_score->__gpio_24_val__)
//GPIO_VAL_SET(gpio_sus->__gpio_25_val__)
GPIO_VAL_SET(gpio_score->__gpio_26_val__)


printf("%08x\n", gpio_score->__gpio_05_val__);
	printf("%08x\n", gpio_score->__gpio_06_val__);
	printf("%08x\n", gpio_score->__gpio_07_val__);
	printf("%08x\n", gpio_score->__gpio_08_val__);
	printf("%08x\n", gpio_score->__gpio_09_val__);
	printf("%08x\n", gpio_score->__gpio_10_val__);
	printf("%08x\n", gpio_score->__gpio_11_val__);
	printf("%08x\n", gpio_score->__gpio_12_val__);
	printf("%08x\n", gpio_score->__gpio_13_val__);
	printf("%08x\n", gpio_score->__gpio_14_val__);
	printf("%08x\n", gpio_score->__gpio_15_val__);
	printf("%08x\n", gpio_score->__gpio_16_val__);
	printf("%08x\n", gpio_score->__gpio_17_val__);
	printf("%08x\n", gpio_score->__gpio_18_val__);
	printf("%08x\n", gpio_score->__gpio_19_val__);
	printf("%08x\n", gpio_score->__gpio_20_val__);
	printf("%08x\n", gpio_score->__gpio_22_val__);
	printf("%08x\n", gpio_score->__gpio_24_val__);
	printf("%08x\n", gpio_score->__gpio_26_val__);

	//printf("%08x\n", gpio_map->__gpio_22_cfg__);
	//printf("%08x\n\n", gpio_map->__gpio_22_val__);
	//GPIO_CFG_FUNCTION(gpio_map->__gpio_22_cfg__,1);
	//PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,20);
	//PWM_CTRL_BASE_UNIT(pwm_map->__pwm_ctrl__,8);

	//PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
	//PWM_CTRL_ENABLE(pwm_map->__pwm_ctrl__);

	/*for ( int i = 0 ; i <= 255 ; i++ ){
		PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,i)
		PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
		usleep(10000);
	}
	PRINT_PWM_STATUS(pwm_map->__pwm_ctrl__);

	for ( int i = 254 ; i >= 0 ; i-- ){
		PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,i)
		PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
		usleep(10000);
	}
	PRINT_PWM_STATUS(pwm_map->__pwm_ctrl__);

*/

/*
	for (int i=0; i< 20000 ; i++){
		PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,80);
		usleep(10);
		PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,80);
		usleep(11);
		PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,80);
		usleep(10);

		PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,255);
		usleep(51);

	}
*/
	UNMAP_DEVICE(gpio_score, sizeof(gpio_score_t));

	//UNMAP_DEVICE(gpio_score, sizeof(gpio_sus_t));
		//UNMAP_DEVICE(gpio_map);

return 0;
}
