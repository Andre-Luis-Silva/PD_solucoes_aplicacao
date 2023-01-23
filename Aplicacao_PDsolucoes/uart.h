/*
 * uart.h
 *
 * Created: 17/01/2023 22:58:57
 *  Author: andre
 */ 


#ifndef UART_H_
#define UART_H_

#define FOSC 8000000 // Clock Speed
#define BAUD 9600

#define PULA_LINHA	EscritaTexto("\r",sizeof("\r"));
void ConfigUart( void );
unsigned char LeSerial( void );
void EscritaTexto( char *caracter, unsigned char tamanho );



#endif /* UART_H_ */