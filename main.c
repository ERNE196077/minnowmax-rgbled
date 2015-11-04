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

	printf("Hello World! Biatch\n");

	volatile gpio_t *gpio_map = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS);
	volatile pwm_t *pwm_map = MAP_DEVICE(PWM0_BASE_ADDR);


	printf("%08x\n", gpio_map->__gpio_22_cfg__);
	printf("%08x\n\n", gpio_map->__gpio_22_val__);

	PWM_CTRL_BASE_UNIT(pwm_map->__pwm_ctrl__,8);
	PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
	PWM_CTRL_ENABLE(pwm_map->__pwm_ctrl__);

	for ( int i = 0 ; i <= 255 ; i++ ){
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



	UNMAP_DEVICE(pwm_map);
	UNMAP_DEVICE(gpio_map);

return 0;
}
