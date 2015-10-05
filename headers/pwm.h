#ifndef PWM_H_
#define PWM_H_
#endif /* GPIO_H_ */


/*****			GPIO BASE REGISTERS			*****/

#define 	PWM_BLOCK_SIZE	(4*1024)
#define		PWM0_PIN_NUM	22
#define 	PWM0_BASE_ADDR	0x90825000
#define 	PWM0_CTRL_OFF	(0x0 / 4)
#define 	PWM0_RES_OFF	(804 / 4)
#define 	PWM0_GEN_OFF	(808 / 4)
#define 	PWM1_PIN_NUM	24
#define		PWM1_BASE_ADDR	0x90827000 
#define 	PWM1_CTRL_OFF	(0x0 / 4)
#define 	PWM1_RES_OFF	(804 / 4)
#define 	PWM1_GEN_OFF	(808 / 4)


int 	INIT_PWM0			();
int 	STOP_PWM0			();
int 	SET_PWM0_DUTY		(u_int16_t __duty__);
int 	SET_PWM0_DIV		(u_int8_t __div__);
void 	PRINT_PWM0_REG		();

int 	INIT_PWM1			();
int 	STOP_PWM1			();
int 	SET_PWM1_DUTY		(u_int16_t __duty__);
int 	SET_PWM1_DIV		(u_int8_t __div__);
void 	PRINT_PWM1_REG		();



