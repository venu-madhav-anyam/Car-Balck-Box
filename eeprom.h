/*
 * File:   eeprom.h
 * Author: venu
 *
 * Created on 21 September, 2024, 7:29 AM
 */

#ifndef EEPROM_H
#define EEPROM_H


void write_eeprom(unsigned char address1, unsigned char data);
unsigned char read_eeprom(unsigned char address1);

void write_internal_eeprom(unsigned char address, unsigned char data); 
unsigned char read_internal_eeprom(unsigned char address);


#endif
