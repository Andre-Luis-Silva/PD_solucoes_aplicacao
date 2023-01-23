/*
 * gpio.c
 *
 * Created: 17/01/2023 22:59:16
 *  Author: andre
 */ 
#include <comum.h>

void ConfigGpio( void ){
	
	DDRC = (1 << DDC1) | (1 << DDC0);  // Define PC0 e PC1 como saída
	PORTC = (1 << PORTC3) | (1 << PORTC2);	// Define PC2 e PC3 com Pull-up	
	LED1_OFF;
	LED2_OFF;
	
}