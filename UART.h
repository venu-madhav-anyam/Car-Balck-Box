/* 
 * File:   UART.h
 * Author: venu
 *
 * Created on 24 September, 2024, 5:52 PM
 */

#ifndef SCI_H
#define SCI_H

#define RX_PIN					TRISC7
#define TX_PIN					TRISC6

void init_uart(void);
void putch(unsigned char byte);
int puts(const char *s);     
unsigned char getch(void); 
unsigned char getche(void);   

#endif
