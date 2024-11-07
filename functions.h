#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "main.h"

static void get_time(void);
void cursor_blinking(int i);
void dashboard(unsigned char key, press_type key_type);
void menu_screen(unsigned char key, press_type key_type);
void password(unsigned char key, press_type key_type);
void capture_event(unsigned char key, press_type key_type);
void view_logs(unsigned char key, press_type key_type);
void upload_logs(unsigned char **logs);
void download_logs(unsigned char **logs);
void reset_password(unsigned char key, press_type key_type);
void transmit_logs(void);
void clear_logs(void);
void record_event(unsigned char ch1, unsigned char ch2);
void reset_time(unsigned int key, select_opt key_type);

#endif



// clcd_putch('0' + i, LINE2(0));

    // clcd_putch(' ', LINE2(1));
    // clcd_putch('0' + ((logs[i][0] >> 4) & 0x01), LINE2(2));
    // clcd_putch('0' + (logs[i][0] & 0x0F), LINE2(3));
    // clcd_putch(':', LINE2(4));
    // clcd_putch('0' + ((logs[i][1] >> 4) & 0x01), LINE2(5));
    // clcd_putch('0' + (logs[i][1] & 0x01), LINE2(6));
    // clcd_putch(':', LINE2(7));
    // clcd_putch('0' + ((logs[i][2] >> 4) & 0x01), LINE2(8));
    // clcd_putch('0' + (logs[i][2] & 0x01), LINE2(9));

    // clcd_putch(' ', LINE2(10));
    // clcd_putch(logs[i][3], LINE2(11));
    // clcd_putch(logs[i][4], LINE2(12));

    // clcd_putch(' ', LINE2(13));
    // clcd_putch('0' + ((logs[i][5] - '0') / 10), LINE2(14));
    // clcd_putch('0' + ((logs[i][5] - '0') % 10), LINE2(15));