/*
 * gpio.h
 *
 * Created: 17/01/2023 22:59:28
 *  Author: andre
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#define LED1_ON			PORTC |= (1 << PORTC0)
#define LED1_OFF		PORTC &= ~(1 << PORTC0)
#define LED1_TOOGLE		PORTC ^= (1 << PORTC0)
#define LED2_ON			PORTC |= (1 << PORTC1)
#define LED2_OFF		PORTC &= ~(1 << PORTC1)
#define LED2_TOOGLE		PORTC ^= (1 << PORTC1)
#define BOT1_PRESS		(PINC & (1 << PINC2))
#define BOT2_PRESS		(PINC & (1 << PINC3))
#define PRESS			0
#define NAO_PRESS		1
void ConfigGpio( void );

#endif /* GPIO_H_ */