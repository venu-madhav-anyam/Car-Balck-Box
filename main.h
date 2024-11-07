#ifndef MAIN_H
#define MAIN_H

int i=0,wait=0,blink=0,wait1=0,count=0,speed=0,g=-1,hold=0,crash = 0,option=99,press=0,pass_read=1;
unsigned char prekey=0,key=0;

unsigned char gears[] = {'R','N','1','2','3','4','5'};
int h,m,s;

unsigned char op[5]="0000";
unsigned char pass[9],star='*';
unsigned char time[9];
unsigned char clock_reg[3];

unsigned char list[5][16]={"View Logs      ",
                           "Download Logs  ",
                           "Clear Logs     ",
                           "Reset Time     ",
                           "Change Password"};


unsigned int write=0,log=0;
unsigned char logs[11][17];

char dwnl[]="/ / / / / / / / ";

typedef enum 
{
    Dash_Board,
    Password,
    Menu_Screen,
    View_Logs,
    Download_Logs,
    Clear_Logs,
    Reset_Time,
    Change_Password
}select_opt;

select_opt select = Dash_Board;

typedef enum
{
    edge,
    level,
    no_press
}press_type;

press_type key_pressed();


#endif