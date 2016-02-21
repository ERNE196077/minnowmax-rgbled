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

	volatile u_int32_t *base = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS, BLOCK_SIZE);

	volatile gpio_t *gpio6 = GPIO_MAP(6,base)
		//printf("%08x\n\n", gpio_map->__gpio_22_val__);
	//GPIO_CFG_FUNCTION(gpio6->__gpio_cfg__,0);
	//GPIO_VAL_OUTPUT(gpio6->__gpio_val__);
	GPIO_VAL_CLR(gpio6->__val__);


			printf("%08x\n", gpio6->__val__);
			printf("%08x\n", gpio6->__cfg__);
				//PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,20);
	//PWM_CTRL_BASE_UNIT(pwm_map->__pwm_ctrl__,8);

	//PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
	//PWM_CTRL_ENABLE(pwm_map->__pwm_ctrl__);

			UNMAP_DEVICE(base,4096);

return 0;
}
