/*
 * File:   timer.c
 * Author: venu
 *
 * Created on 18 September, 2024, 4:15 PM
 */


#include <xc.h>
#include "timer.h"

void init_timer(void)
{

	T08BIT = 1;

	T0CS = 0;

	TMR0ON = 1;

	PSA = 1;

	TMR0 = 6;

	TMR0IF = 0;

	TMR0IE = 1;
}