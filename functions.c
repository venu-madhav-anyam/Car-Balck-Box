#include "functions.h"
#include "isr.h"
#include "main.h"
#include "clcd.h"
#include "mkp.h"
#include "eeprom.h"
#include <string.h>
#include "rtc.h"
#include "i2c.h"
#include "UART.h"

static void get_time(void)
{
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
    time[1] = '0' + (clock_reg[0] & 0x0F);
    time[2] = ':';
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
    time[4] = '0' + (clock_reg[1] & 0x0F);
    time[5] = ':';
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
    time[7] = '0' + (clock_reg[2] & 0x0F);
    time[8] = '\0';
}

// Cursor Blinking
void cursor_blinking(int i)
{
    if (blink)
        clcd_putch(' ', LINE2(i));
    else
        clcd_putch('_', LINE2(i));
}

// Uploading events to EEPROM
void upload_logs(unsigned char **logs)
{
    unsigned int wadd = 0;

    for (int i = 0; i < (write + 1); i++)
        for (int j = 0; j < 17; j++)
            write_eeprom(wadd++, logs[i][j]);
    write++;
}

// Downloading Events From EEPROM
void download_logs(unsigned char **logs)
{
    unsigned int d_add = 0;

    for (int i = 0; i < write; i++)
        for (int j = 0; j < 17; j++)
            logs[i][j] = read_eeprom(d_add++);
}

// Dashboard
void dashboard(unsigned char key, press_type key_type)
{

    clcd_print("  TIME   EV  SPD", LINE1(0));

    get_time();
    clcd_print(time, LINE2(0));

    if (g == -1 && !crash)
        clcd_print(" ON ", LINE2(8));
    else if (crash)
        clcd_print(" C! ", LINE2(8)), g = 0;
    else
    {
        clcd_putch(' ', LINE2(8));
        clcd_putch('G', LINE2(9));
        clcd_putch(gears[g], LINE2(10));
        clcd_putch(' ', LINE2(11));
    }

    {
        clcd_putch(' ', LINE2(12));
        clcd_putch('0' + (speed / 10), LINE2(13));
        clcd_putch('0' + (speed % 10), LINE2(14));
        clcd_putch(' ', LINE2(15));
    }

    if (key == MK_SW5 && key_type == edge)
    {
        prekey = 0;
        select = Password;
        CLEAR_DISP_SCREEN;
    }
}

// Login Screen
void password(unsigned char key, press_type key_type)
{
    // op[0] = read_internal_eeprom(0);
    // op[1] = read_internal_eeprom(1);
    // op[2] = read_internal_eeprom(2);
    // op[3] = read_internal_eeprom(3);
    // op[4] = '\0';

    if (count < 3)
    {
        if (i < 4)
        {
            cursor_blinking(i);

            { // Reading & storing Passkey

                clcd_print(" ENTER PASSWORD ", LINE1(0));

                if (key == MK_SW5 && key_type == edge)
                    pass[i++] = '0', login = 0, prekey = 0;
                else if (key == MK_SW6 && key_type == edge)
                    pass[i++] = '1', login = 0, prekey = 0;

                if (i > 0)
                    clcd_putch('*', LINE2(i - 1));

                if (login > 5)
                    select = Dash_Board;
            }
        }

        { // Checking passkey
            if (i == 4)
            {
                pass[4] = '\0';
                if (strcmp(pass,op) == 0)
                {
                    pass[0] = '/0';
                    i = 0;
                    count = 0;
                    select = Menu_Screen;
                }
                else if (strcmp(op, pass) != 0)
                {
                    if (2 - count)
                    {
                        clcd_print(" PASWD IS WRONG ", LINE1(0));
                        clcd_putch('0' + (2 - count), LINE2(0));
                        clcd_print("  TRIES LEFT   ", LINE2(1));
                    }

                    if (wait++ == 1200)
                    {
                        wait = 0;
                        i = 0;
                        count++;
                        CLEAR_DISP_SCREEN;
                    }
                }
            }
        }
        delay = 31;
    }
    else
    {
        clcd_print(" ACCESS DENIED  ", LINE1(0));
        clcd_print("Try After ", LINE2(0));
        clcd_putch('0' + (delay / 10), LINE2(10));
        clcd_putch('0' + (delay % 10), LINE2(11));
        clcd_print(" Sec", LINE2(12));

        if (!delay)
            count = 0;
    }
}

// Menu Screen
void menu_screen(unsigned char key, press_type key_type)
{
    static unsigned int ind = 0, sel = 1;

    clcd_print(list[ind], LINE1(1));
    clcd_print(list[(ind + 1)], LINE2(1));

    if (key == MK_SW5 && (ind >= 0) && key_type == edge)
    {
        prekey = 0;

        if (sel && (ind >= 1))
            ind--;
        else
            sel = 1;
    }

    if (key == MK_SW6 && (ind <= 3) && key_type == edge)
    {
        prekey = 0;

        if ((!sel) && (ind <= 2))
            ind++;
        else
            sel = 0;
    }

    if (sel)
    {
        clcd_putch(0x7E, LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    else if (!sel)
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch(0x7E, LINE2(0));
    }

    // static unsigned int select = 0;

    if (key == MK_SW5 && key_type == level)
    {
        prekey = 0;

        if (sel)
            option = ind;
        else if (!sel)
            option = ind + 1;
    }

    if (option == 0)
    {
        option = 99;
        select = View_Logs;
        CLEAR_DISP_SCREEN;
    }
    else if (option == 1)
    {
        option = 99;
        select = Download_Logs;
        CLEAR_DISP_SCREEN;
    }
    else if (option == 2)
    {
        option = 99;
        select = Clear_Logs;
        CLEAR_DISP_SCREEN;
    }
    else if (option == 3)
    {
        option = 99;
        select = Reset_Time;
        CLEAR_DISP_SCREEN;
    }
    else if (option == 4)
    {
        option = 99;
        select = Change_Password;
        CLEAR_DISP_SCREEN;
    }

    if (key == MK_SW6 && key_type == level)
    {
        prekey = 0;
        ind = 0, sel = 1;
        select = Dash_Board;
    }
}

// View logs screen
void view_logs(unsigned char key, press_type key_type)
{
    static unsigned int i = 0, st = 1;

    if (st)
        st = 0, download_logs(logs);

    clcd_putch(0xFF, LINE1(0));
    clcd_print(" Log  Details ", LINE1(1));
    clcd_putch(0xFF, LINE1(15));
    clcd_print(logs[i], LINE2(0));

    if ((key == MK_SW5) && (i > 0) && (key_type == edge))
        i--, prekey = 0;
    if ((key == MK_SW6) && (i < write - 1) && (key_type == edge))
        i++, prekey = 0;

    if (key == MK_SW6 && key_type == level)
    {
        prekey = 0;
        i = 0;
        st = 1;
        select = Menu_Screen;
    }
}

// Download AND Transmit logs
void transmit_logs()
{
    static int j = 0, once = 1;

    if (once)
    {
        record_event('D', 'L');
        once = 0, puts("LOGS RECORDED IN THE BLACK BOX ARE :-\n\r");
        download_logs(logs);
    }

    clcd_print("LOGS DOWNLOADING", LINE1(0));

    clcd_print(dwnl, LINE2(0));

    while (j < write)
    {
        puts(logs[j++]);
        puts("\n\r");
    }

    if (hold++ == 1000)
    {
        hold = 0, once = 1, j = 0;
        select = Menu_Screen;
    }
}

// Clear Logs
void clear_logs()
{
    write = 0;
    log = 0;
    clcd_print("  LOGS CLEARED  ", LINE1(0));
    clcd_print("  SUCCESSFULLY  ", LINE2(0));

    if (hold++ == 600)
    {
        hold = 0;
        record_event('C', 'L');
        select = Menu_Screen;
    }
}

// Recording Events
void record_event(unsigned char ch1, unsigned char ch2)
{
    if (log > 9)
    {
        for (int i = 0; i < 9; i++)
        {
            strcpy(logs[i], logs[i + 1]);
            logs[i][0] = '0' + i;
        }
        log--;
        write--;
    }

    logs[log][0] = '0' + log;
    logs[log][1] = ' ';
    strncat(logs[log], time, 8);
    logs[log][10] = ' ';
    logs[log][11] = ch1;
    logs[log][12] = ch2;
    logs[log][13] = ' ';
    logs[log][14] = '0' + speed / 10;
    logs[log][15] = '0' + speed % 10;
    logs[log++][16] = '\0';

    upload_logs(logs);
}

// Change Password screen
void reset_password(unsigned char key, press_type key_type)
{
    static char newpass[5], reenter[5];
    static int i = 0, j = 0, one = 1;
    if (one)
    {
        key = 0;
        one = 0;
    }

    if (i < 4)
    {
        cursor_blinking(i);

        { // Reading & storing Passkey

            clcd_print(" ENTER NEW PSWD ", LINE1(0));

            if (key == MK_SW5 && key_type == edge)
                newpass[i++] = '0', key = 0, prekey = 0;
            else if (key == MK_SW6 && key_type == edge)
                newpass[i++] = '1', key = 0, prekey = 0;

            if (i > 0)
                clcd_putch('*', LINE2(i - 1));
        }
    }

    static int clear = 1;
    { // Re-enter
        if (i == 4 && j < 4)
        {

            if (clear)
                clear = 0, CLEAR_DISP_SCREEN;

            cursor_blinking(j);

            clcd_print(" RE-ENTER PASWD ", LINE1(0));

            if (key == MK_SW5 && key_type == edge)
                reenter[j++] = '0', prekey = 0;
            else if (key == MK_SW6 && key_type == edge)
                reenter[j++] = '1', prekey = 0;

            if (j > 0)
                clcd_putch('*', LINE2(j - 1));
        }
    }

    { // Checking passkey
        if (i == 4 && j == 4)
        {
            reenter[j] = newpass[i] = '\0';

            if (strcmp(newpass, reenter) == 0)
            {
                clcd_print("PASSWORD CHANGED", LINE1(0));
                clcd_print("  SUCCESSFULLY  ", LINE2(0));
                if (wait++ == 800)
                {
                    i = 0, j = 0, wait = 0, clear = 1, pass_read = 1, one = 1;
                    strcpy(op, reenter);

                    write_internal_eeprom(0, op[0]);
                    write_internal_eeprom(1, op[1]);
                    write_internal_eeprom(2, op[2]);
                    write_internal_eeprom(3, op[3]);

                    record_event('C', 'P');

                    select = Dash_Board;
                }
            }
            else if (strcmp(newpass, reenter) != 0)
            {
                clcd_print("PSWD NOT MATCHED", LINE1(0));
                clcd_print("   TRY  AGAIN   ", LINE2(0));

                if (wait++ >= 1500)
                {
                    i = 0, j = 0;
                    wait = 0, clear = 1;
                    select = Menu_Screen;
                }
            }
        }
    }

    if (key == MK_SW6 && key_type == level)
    {
        prekey = 0;
        select = Menu_Screen;
    }
}

void reset_time(unsigned int key, select_opt key_type)
{
    static int once = 1, field = 0;

    if (once)
    {
        h = ((clock_reg[0] >> 4 & 0x03) * 10) + clock_reg[0] & 15;
        m = ((clock_reg[1] >> 4) * 10) + clock_reg[1] & 15;
        s = ((clock_reg[2] >> 4) * 10) + clock_reg[2] & 15;
        once = 0;
    }

    clcd_putch(0xFF, LINE1(0));
    clcd_print("  Reset Time  ", LINE1(1));
    clcd_putch(0xFF, LINE1(15));

    if (field == 2)
    {
        if (blink)
        {
            clcd_putch(' ', LINE2(4));
            clcd_putch(' ', LINE2(5));
        }
        else
        {
            clcd_putch('0' + (h / 10), LINE2(4));
            clcd_putch('0' + (h % 10), LINE2(5));
        }

        clcd_putch(':', LINE2(6));
        clcd_putch('0' + (m / 10), LINE2(7));
        clcd_putch('0' + (m % 10), LINE2(8));
        clcd_putch(':', LINE2(9));
        clcd_putch('0' + (s / 10), LINE2(10));
        clcd_putch('0' + (s % 10), LINE2(11));
    }

    if (field == 1)
    {
        if (blink)
        {
            clcd_putch(' ', LINE2(7));
            clcd_putch(' ', LINE2(8));
        }
        else if (!blink)
        {
            clcd_putch('0' + (m / 10), LINE2(7));
            clcd_putch('0' + (m % 10), LINE2(8));
        }

        clcd_putch('0' + (h / 10), LINE2(4));
        clcd_putch('0' + (h % 10), LINE2(5));
        clcd_putch(':', LINE2(6));

        clcd_putch(':', LINE2(9));
        clcd_putch('0' + (s / 10), LINE2(10));
        clcd_putch('0' + (s % 10), LINE2(11));
    }

    if (field == 0)
    {
        if (blink)
        {
            clcd_putch(' ', LINE2(10));
            clcd_putch(' ', LINE2(11));
        }
        else if (!blink)
        {
            clcd_putch('0' + (s / 10), LINE2(10));
            clcd_putch('0' + (s % 10), LINE2(11));
        }

        clcd_putch('0' + (h / 10), LINE2(4));
        clcd_putch('0' + (h % 10), LINE2(5));
        clcd_putch(':', LINE2(6));
        clcd_putch('0' + (m / 10), LINE2(7));
        clcd_putch('0' + (m % 10), LINE2(8));
        clcd_putch(':', LINE2(9));
    }

    if (key == MK_SW5 && key_type == edge)
    {
        prekey = 0;

        if (field == 2)
        {
            if (h < 23)
                h++;
            else
                h = 0;
        }

        if (field == 1)
        {
            if (m < 59)
                m++;
            else
                m = 0;
        }

        if (field == 0)
        {
            if (s < 59)
                s++;
            else
                s = 0;
        }
    }

    if (key == MK_SW6 && key_type == edge)
    {
        prekey = 0;

        if (field < 2)
            field++;
        else
            field = 0;
    }

    if (key == MK_SW5 && key_type == level)
    {
        clock_reg[0] = (h / 10);
        clock_reg[0] <<= 4;
        clock_reg[0] |= (h % 10);

        clock_reg[1] = (m / 10);
        clock_reg[1] <<= 4;
        clock_reg[1] |= (m % 10);

        clock_reg[2] = (s / 10);
        clock_reg[2] <<= 4;
        clock_reg[2] |= (s % 10);

        write_ds1307(HOUR_ADDR, clock_reg[0]);
        write_ds1307(MIN_ADDR, clock_reg[1]);
        write_ds1307(SEC_ADDR, clock_reg[2]);

        record_event('R', 'T');

        select = Menu_Screen;
    }

    if (key == MK_SW6 && key_type == level)
    {
        prekey = 0;
        CLEAR_DISP_SCREEN;
        once = 1;
        select = Menu_Screen;
    }
}
