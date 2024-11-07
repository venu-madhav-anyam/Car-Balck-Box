/*
 * File:   eeprom.c
 * Author: venu
 *
 * Created on 21 September, 2024, 7:29 AM
 */

#include "i2c.h"
#include "eeprom.h"
#include <xc.h>

#define EEPROM_READ 0xA1
#define EEPROM_WRITE 0xA0


void write_eeprom(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(EEPROM_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
}

unsigned char read_eeprom(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(EEPROM_WRITE);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(EEPROM_READ);
	data = i2c_read();
	i2c_stop();

	return data;
}


void write_internal_eeprom(unsigned char address, unsigned char data) 
{
	//Write the address
	EEADR = address;

	//Write the data
	EEDATA = data;

	//Point to data memory
	EECON1bits.EEPGD = 0;

	//Access data EEPROM memory
	EECON1bits.CFGS = 0;

	//Data write enable bit
	EECON1bits.WREN = 1;

	//Global interrupt disable 
	GIE = 0;

	//Write protection sequence
	EECON2 = 0x55;
	EECON2 = 0xAA;

	//Initiate write
	EECON1bits.WR = 1;

	//Global interrupt enable
	GIE = 1;

	//Wait till write is complete
	while (!PIR2bits.EEIF);

	//Disable the flag
	PIR2bits.EEIF = 0;
}

unsigned char read_internal_eeprom(unsigned char address)
{
	//Write the address, from where data has to be read
	EEADR = address;

	//Inhibits write cycles to Flash program/data EEPROM
	EECON1bits.WREN = 0;

	//Point to data memory
	EECON1bits.EEPGD = 0;

	//Access data EEPROM memory
	EECON1bits.CFGS = 0;
	
	//Initiate read
	EECON1bits.RD = 1;

	//Data available in EEDATA register
	return EEDATA;
}
