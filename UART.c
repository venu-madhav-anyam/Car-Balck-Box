/*
 * File:   UART.c
 * Author: venu
 *
 * Created on 24 September, 2024, 5:52 PM
 */


#include <xc.h>
#include "UART.h"

void init_uart(void)
{
	RX_PIN = 1;
	TX_PIN = 0;

	TX9 = 0;
	TXEN = 1;
	SYNC = 0;
	SENDB = 0;
	BRGH = 1;

	SPEN = 1;
	RX9 = 0; 
	CREN = 1;
 
	BRG16 = 0;
	

	SPBRG = 129;

	TXIF = 0;

	RCIF = 0;
}

void putch(unsigned char byte) 
{
	while(!TXIF) 
	{
		continue;
	}
	TXIF = 0;
	TXREG = byte;
} 

int puts(const char *s)
{
	while(*s)		
	{
		putch(*s++);
	}
	return 0;
}

unsigned char getch(void)
{
	while(!RCIF) 
	{
		continue;
	}
	RCIF = 0;
	return RCREG;
}

unsigned char getche(void)
{
	unsigned char c;

	putch(c = getch()); 

	return (c);
}

