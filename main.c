/*
 * File:   main.c
 * Author: venu
 *
 * Created on 18 September, 2024, 4:17 PM
 */

#include <xc.h>
#include <string.h>
#include "main.h"
#include "mkp.h"
#include "clcd.h"
#include "timer.h"
#include "adc.h"
#include "isr.h"
#include "functions.h"
#include "rtc.h"
#include "i2c.h"
#include "UART.h"
#include "eeprom.h"

void init_config(void)
{
    init_matrix_keypad();
    init_clcd();
    init_timer();
    init_adc();
    init_i2c();
    init_rtc();
    init_uart();
    PEIE = 1;
    GIE = 1;
}

press_type key_pressed()
{
    static int press = 0;

    prekey = key;
    key = read_switches(LEVEL_CHANGE);

    if (key != ALL_RELEASED)
    {
        if (press++ > 400)
        {
            press = 0;
            return level;
        }
    }
    else if (400 > press && press > 0)
    {
        press = 0;
        return edge;
    }

    return no_press;
}

void main(void)
{
    init_config();

    press_type key_type;

    while (1)
    {
        key_type = key_pressed();

        speed = (read_adc(CHANNEL4)) / 10.3;

        { // Dashboard

            if (select == Dash_Board)
                dashboard(prekey, key_type);
        }

        { // Gear change
            if (prekey == MK_SW2 && (g < 6 || g == -1) && key_type == edge)
            {
                prekey = 0;
                g++, crash = 0;
                record_event('G', gears[g]);
            }
            if (prekey == MK_SW3 && g > 0 && g != -1 && key_type == edge)
            {
                prekey = 0;
                g--, crash = 0;
                record_event('G', gears[g]);
            }
            if (prekey == MK_SW1 && key_type == edge)
            {
                prekey = 0;
                g = 1;
                crash = 1;
                record_event('C', '!');
            }
        }

        { // Event Capture

            static int init = 1;

            if (init)
            {
                record_event('O', 'N');

                init = 0;
            }

            if (wait1++ == 400)
            {
                wait1 = 0;
                blink = !blink;
            }
        }

        { // login screen password
            if (select == Password)
                password(prekey, key_type);
            else
                login = -1;
        }

        { // Menu
            if (select == Menu_Screen)
                menu_screen(prekey, key_type);
        }

        { // View log

            if (select == View_Logs)
                view_logs(prekey, key_type);
        }

        { // Downloading & transmitting Logs
            if (select == Download_Logs)
            {
                transmit_logs();
            }
        }

        { // Clearing Logs
            if (select == Clear_Logs)
                clear_logs();
        }

        { // Reset Time
            if (select == Reset_Time)
                reset_time(prekey, key_type);
        }

        { // Change Password
            if (select == Change_Password)
                reset_password(prekey, key_type);
        }
    }
}
