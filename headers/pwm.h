#ifndef PWM_H_
#define PWM_H_
#endif /* GPIO_H_ */


/*****			GPIO BASE REGISTERS			*****/

#define PWM_BLOCK_SIZE			(4*1024)
#define	PWM_CTL_ADDRESS 	0xFED0C000

#define PWM0_OFFSET			0x0

int 	INIT_PWM			();
int 	CLOSE_PWM			(int pin_n);
void 	READ_PWM_REG		(int pin_n);
u_int16_t pciConfigReadWord (u_int8_t bus, u_int8_t slot, u_int8_t func, u_int8_t offset);
