/*
 * File:   adc.c
 * Author: venu
 *
 * Created on 18 September, 2024, 4:15 PM
 */


#include <xc.h>
#include "adc.h"

void init_adc(void)
{
	ADFM = 1;

	ACQT2 = 1;
	ACQT1 = 0;
	ACQT0 = 0;

	ADCS0 = 0;
	ADCS1 = 1;
	ADCS2 = 0;

	GODONE = 0;

	

	VCFG1 = 0;
	VCFG0 = 0;

	ADRESH = 0;
	ADRESL = 0;

	ADON = 1;
}

unsigned short read_adc(unsigned char channel)
{
	unsigned short reg_val;

	ADCON0 = (ADCON0 & 0xC3) | (channel << 2);

	GO = 1;
	while (GO);
	reg_val = (ADRESH << 8) | ADRESL; 

	return reg_val;
}