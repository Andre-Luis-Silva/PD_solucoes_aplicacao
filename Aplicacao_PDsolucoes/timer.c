/*
 * timer.c
 *
 * Created: 23/01/2023 02:16:02
 *  Author: andre
 */ 
#include <comum.h>

unsigned int timer1segundo = 0;
unsigned char timeoutPorta = 0;
extern unsigned char g_flagPortaAtiva, flagPorta1, flagPorta2;

void TimerConfig( void )
{
	
	TCCR0A = 0; // Conta até o valor de comparação OCR0A
	TCCR0B = ( 1 << CS01 ) | ( 1 << CS00 ); // Clock sem prescaler
	TIFR0 = 1 << TOV0;
	TIMSK0 |= 1 << TOIE0;
	TCNT0 = 0;
	
}

ISR( TIMER0_OVF_vect )
{
	
	TCNT0 = 255 - 125;
	if( (flagPorta1 == 1) && (BOT1_PRESS == PRESS))
	{
		g_flagPortaAtiva = 0;
		timeoutPorta = 0;
		flagPorta1 = 0;
		LED1_OFF;
	}
	if( (flagPorta2 == 1) && (BOT2_PRESS == PRESS))
	{
		g_flagPortaAtiva = 0;
		timeoutPorta = 0;
		flagPorta2 = 0;
		LED2_OFF;
	}
	if( timer1segundo < 1000 )
	{
		timer1segundo++;
	}
	else
	{
		timer1segundo = 0;
		if( g_flagPortaAtiva )
		{
			timeoutPorta++;
			if( (timeoutPorta >= 5) )
			{
				g_flagPortaAtiva = 0;
				timeoutPorta = 0;
				LED1_OFF;
				LED2_OFF;
			}
		}
	}
	
}