#ifndef PWM_H_
#define PWM_H_
#endif /* GPIO_H_ */


/*****			PWM BASE REGISTERS			*****/

#define 	PWM_BLOCK_SIZE	(4*1024)
#define 	PWM0_BASE_ADDR	0x90825000
#define		PWM1_BASE_ADDR	0x90827000 
#define		PWM0_PIN_NUM	22
#define 	PWM1_PIN_NUM	24


/*****			PWM MACROS			*****/

#define 	PWM_CTRL_ENABLE(pwm_reg)			pwm_reg |= (1 << 31)
#define 	PWM_CTRL_DISABLE(pwm_reg)			pwm_reg &= ~(1 << 31)
#define 	PWM_CTRL_BASE_UNIT(pwm_reg,value)	pwm_reg &= ~(0xFFFF << 8); pwm_reg |= (value << 8)
#define 	PWM_CTRL_ON_DIVISOR(pwm_reg,value)	pwm_reg &= ~0xFF; pwm_reg


/*****			PWM STRUCTURES			*****/

typedef struct {
	u_int32_t __pwm_ctrl__;
	u_int32_t __resv_0x0004__[512];
	u_int32_t __pwm_rest__;
	u_int32_t __pwm_greg__;
	u_int32_t __resv_0x080C__[509];
}__attribute__ ((packed)) pwm_t ;

