/*
 * uart.c
 *
 * Created: 17/01/2023 22:58:36
 *  Author: andre
 */ 

#include <comum.h>

unsigned char recebeuDado = 0;

void ConfigUart( void )	
{
	/*
	 Configuração da Uart. Configurado com baudrate de 9600
	 Relação de cálculo de BaudRate = 8000000/(9600 * 16) - 1 = 51
	*/
	UBRR0H = (unsigned char)(0);	// Define os 8 bits mais significativos do Baud Rate
	UBRR0L = (unsigned char)51;		// Define os 8 bits menos significativos do Baud Rate
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);		// Habilita o TX, RX e interrupção da USART
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);	// Dados em 8 bits e 1 bit de parada
	
}

void EscritaTexto( char *caracter, unsigned char tamanho )
{
	while(tamanho > 0)	// Enquanto tamanho maior que 0
	{	
		while (!( UCSR0A & (1<<UDRE0)));	// Enquanto o buffer está cheio
		UDR0 = *caracter++;	// UDRO recebe *ponteiro++ (caracter)
		tamanho--;	// tamanho decrementa 1
	}
}

unsigned char LeSerial( void )
{
	return UDR0;	//Retorna a leitura da Serial
}

ISR( USART_RX_vect )
{
	recebeuDado = LeSerial();	// Flag para indicar que recebeu o dado
}