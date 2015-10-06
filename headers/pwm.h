#ifndef PWM_H_
#define PWM_H_
#endif /* GPIO_H_ */


/*****			PWM BASE REGISTERS			*****/

#define 	PWM_BLOCK_SIZE	(4*1024)
#define 	PWM0_BASE_ADDR	0x90825000
#define		PWM1_BASE_ADDR	0x90827000 
#define		PWM0_PIN_NUM	22
#define 	PWM1_PIN_NUM	24


/*****			PWM REGISTER OFFSETS			*****/

#define 	PWM0_CTRL_OFF	(0x0 / 4)
#define 	PWM0_RES_OFF	(804 / 4)
#define 	PWM0_GEN_OFF	(808 / 4)
#define 	PWM1_CTRL_OFF	(0x0 / 4)
#define 	PWM1_RES_OFF	(804 / 4)
#define 	PWM1_GEN_OFF	(808 / 4)


/*****			PWM STRUCTURES			*****/

typedef struct {
	unsigned long 			__pwm_base_addr_ptr__;
	int 					__pwm_memory_file__;
	void 					*__pwm_memory_map__;
	volatile unsigned int 	*__pwm_memory_address__;
}_pwm_device_t_ ;


/*****			PWM 			*****/

int 	MAP_PWM				(u_int8_t pwm_n);
void 	UNMAP_PWM			(u_int8_t pwm_n);
int 	INIT_PWM			(u_int8_t pwm_n);
int 	STOP_PWM			(u_int8_t pwm_n);
int 	SET_PWM_DUTY		(u_int8_t pwm_n, u_int16_t duty);
int 	SET_PWM_DIV			(u_int8_t pwm_n, u_int8_t div);
void 	PRINT_PWM_STATUS	(u_int8_t pwm_n);




