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

	printf("%ld\n",sysconf(_SC_PAGESIZE));

	volatile gpio_score_t *gpio_score = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS, sizeof(gpio_score_t));
	gpio_t *gpio26 = ((gpio_t *)gpio_score)+(GPIO_26_OFFSET/4);

	printf("%08x\n", gpio26->__gpio_cfg__);
	//printf("%08x\n\n", gpio_map->__gpio_22_val__);
	//GPIO_CFG_FUNCTION(gpio_map->__gpio_22_cfg__,1);
	//PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,20);
	//PWM_CTRL_BASE_UNIT(pwm_map->__pwm_ctrl__,8);

	//PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
	//PWM_CTRL_ENABLE(pwm_map->__pwm_ctrl__);
	UNMAP_DEVICE(gpio_score, sizeof(gpio_score_t));

return 0;
}
