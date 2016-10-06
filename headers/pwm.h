#ifndef PWM_H_
#define PWM_H_


/*****			PWM BASE REGISTERS			*****/

#define 	PWM0_BASE_ADDR	0x90c10000 //90825000
#define		PWM1_BASE_ADDR	0x90c12000 //90827000
#define		PWM0_PIN_NUM	22
#define 	PWM1_PIN_NUM	24


/*****			PWM MACROS			*****/

#define 	PWM_CTRL_ENABLE(pwm_reg)				pwm_reg |= (1 << 31);
#define 	PWM_CTRL_DISABLE(pwm_reg)				pwm_reg &= ~(1 << 31);
#define 	PWM_CTRL_SW_UPDATE(pwm_reg)				pwm_reg |= (1 << 30);
#define 	PWM_CTRL_BASE_UNIT(pwm_reg,value)		pwm_reg &= ~(0xFFFF << 8); pwm_reg |= (value << 8);
#define 	PWM_CTRL_ON_TIME_DIVISOR(pwm_reg,value)	pwm_reg &= ~0xFF; pwm_reg|=value;


/*****			PWM STRUCTURES			*****/

typedef struct {
	uint32_t __pwmctrl__;
	#define 	PWM_ENABLE							(1 << 31)			// Construct without this bit for Disable
	#define 	PWM_SW_UPDATE						(1 << 30)			// Used to update the PWM settings. PWM module will apply the new settings at the end of the current cycle and reset this bit.
	#define 	PWM_BASE_UNIT(value)				(value << 8)		// Min value = 0x0. Max value 0xFFFF (65535).
	#define 	PWM_ON_TIME_DIVISOR(value)			(value)				// Min value = 0x0. Max value 0xFF (255).

}__attribute__ ((packed)) pwm_t;


#endif /* PWM_H_ */