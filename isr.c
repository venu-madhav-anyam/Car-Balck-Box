/*
 * File:   isr.c
 * Author: venu
 *
 * Created on 18 September, 2024, 4:16 PM
 */


#include <xc.h>
#include "isr.h"

void __interrupt() isr(void)
{
	static unsigned short count;
     
	if (TMR0IF)
	{
		TMR0 = TMR0 + 8;

		if (count++ == 20000)
		{
			count = 0;
            login++;
            delay--;
		}
		TMR0IF = 0;
	}
}