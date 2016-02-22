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


	volatile u_int32_t *gpio_base = MAP_DEVICE(GPIO_SCORE_BASE_ADDRESS, GPIO_SCORE_BASE_SIZE);
	volatile pwm_t *pwm_base = MAP_DEVICE(PWM0_BASE_ADDR, sizeof(pwm_t));

	volatile gpio_t *gpio22 = GPIO_MAP(22,gpio_base)
		//printf("%08x\n\n", gpio_map->__gpio_22_val__);
//	GPIO_CFG_FUNCTION(gpio22->__cfg__,1);
//	GPIO_VAL_OUTPUT(gpio22->__val__);
//	GPIO_VAL_SET(gpio22->__val__);
/*	for(int i = 0 ; i < 255 ; i++){
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_BASE_UNIT(3000) | PWM_ON_TIME_DIVISOR(i);
		usleep(70000);
	}
	for(int i = 255 ; i > 0  ; i--){
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE | PWM_BASE_UNIT(3000) | PWM_ON_TIME_DIVISOR(i);
		usleep(70000);
	}

*/
		while(1){
			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
								| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(0);
						usleep(50);
	/////////GREEEEEEEN
			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

			}
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

		}
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

		}
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

		}
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

		}


	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {

		}


	/////////////BLUEEEEEE


	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}


	/////////////RED

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(160);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

/***************** LED 222222222222222222222222222222******//////////
	/////////REEEEEEED
	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
	while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
	}

		/////////////GREEEEN



		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}
				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

				pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
						| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
				while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
				}

		/////////////BLUEEEEEEEEE

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}


		/////////REEEEEEED


		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
				| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
		while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
		}

			/////////////GREEEEN

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(80);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}


			/////////////BLUEEEEEEEEE
			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}
			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

			pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
					| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(180);
			while (pwm_base->__pwm_ctrl__ & PWM_SW_UPDATE) {
			}

	pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
			| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(0);
	usleep(50);
		}
		pwm_base->__pwm_ctrl__ = PWM_ENABLE | PWM_SW_UPDATE
							| PWM_BASE_UNIT(2120) | PWM_ON_TIME_DIVISOR(0);

			printf("%08x\n", gpio22->__val__);
			printf("%08x\n", gpio22->__cfg__);
			printf("%08x\n", pwm_base->__pwm_ctrl__);
				//PWM_CTRL_ON_DIVISOR(pwm_map->__pwm_ctrl__,20);
	//PWM_CTRL_BASE_UNIT(pwm_map->__pwm_ctrl__,8);

	//PWM_CTRL_SW_UPDATE(pwm_map->__pwm_ctrl__);
	//PWM_CTRL_ENABLE(pwm_map->__pwm_ctrl__);

			UNMAP_DEVICE(gpio_base,GPIO_SCORE_BASE_SIZE);
			UNMAP_DEVICE(gpio_base,sizeof(pwm_t));
return 0;
}
