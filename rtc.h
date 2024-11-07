/* 
 * File:   rtc.h
 * Author: venu
 *
 * Created on 24 September, 2024, 1:40 PM
 */

#ifndef Ds1307_H
#define Ds1307_H

#define SLAVE_READ		0xD1
#define SLAVE_WRITE		0xD0


#define SEC_ADDR		0x00
#define MIN_ADDR		0x01
#define HOUR_ADDR		0x02
#define CNTL_ADDR		0x07

void init_rtc(void);
void write_ds1307(unsigned char address1,  unsigned char data);
unsigned char read_ds1307(unsigned char address1);

#endif