/*******************************************************************************
clock.cpp
********************************************************************************
Radio message:
  <T;#;PING;2026;07;10;05;55>

********************************************************************************

*******************************************************************************/

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ping.h"
#include "clock.h"
#include "main.h"
//#include "sensor.h"
#include "msg.h"
#include "r69.h"
#include "atask.h"
#include "alpha.h"
#include "rtc.h"

extern msg_st msg;
extern r69_st r69;
extern main_ctrl_st main_ctrl;
extern ping_st ping;
// extern ping_st ping;


void clock_task(void);

clock_st clock_mgr;

const uint8_t dark_time[MONTHS_PER_YEAR][24] =
{   
    //              0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
    [JANUARY]    = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},  
    [FEBRUARY]   = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},
    [MARCH]      = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},
    [APRIL]      = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},
    [MAY]        = {4, 4, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4},
    [JUNE]       = {2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2},
    [JULY]       = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    [AUGUST]     = {4, 4, 3, 3, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4},
    [SEPTEMBER]  = {5, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 5},
    [OCTOBER]    = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},
    [NOVEMBER]   = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},
    [DECEMBER]   = {5, 5, 5, 5, 5, 5, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5},
};




// atask_st:            = {"Label          ", ival, next, state, prev, cntr, run, task_ptr };
atask_st clock_th       = {"Clock Task     ", 1000,    0,     0,  255,    0,   1, clock_task};

void clock_initialize(void)
{
    atask_add_new(&clock_th);
    clock_mgr.next_minute = millis() + 60000;
    clock_mgr.last_hour = 0;
    clock_mgr.new_hour_event = false;
}

void clock_print_date_time(const struct tm *t)
{ 
    char buff[40];
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S (%Z)", t);
    Serial.println(buff);
 
}

uint8_t clock_get_dark_level(void)
{
    return dark_time[clock_mgr.my_time.tm_mon][clock_mgr.my_time.tm_hour];
}

void clock_set_date_time( msg_st *msgp)
{
    struct tm   tmp_time = {0};
    Serial.println("clock_set_date_time");

    uint8_t errors = 0;
    Serial.printf("Time: %s\n", msgp->raw);
    if((msg.field_count == 8) && (msg.fields[1][0] == '#'))
    {
        tmp_time.tm_year  = (uint16_t)msg_robust_atoi(msgp->fields[3],&errors,2000,2100) -1900;    
        tmp_time.tm_mon   = (uint8_t)msg_robust_atoi(msgp->fields[4],&errors,0,11);    
        tmp_time.tm_mday  = (uint8_t)msg_robust_atoi(msgp->fields[5],&errors,1,31);
        tmp_time.tm_hour  = (uint8_t)msg_robust_atoi(msgp->fields[6],&errors,0,23);
        tmp_time.tm_min   = (uint8_t)msg_robust_atoi(msgp->fields[7],&errors,0,59);
        tmp_time.tm_sec = 0;
        tmp_time.tm_isdst = -1;
        Serial.println();
        Serial.printf("time errors %d\n",errors);
        clock_print_date_time(&tmp_time);
        if (errors==0) {
            time_t t = mktime(&tmp_time);
            // struct tm *tmp = localtime(&t);
            clock_mgr.time_p = localtime(&t);
            if (clock_mgr.time_p != NULL)
            {
                clock_mgr.my_time = *clock_mgr.time_p;
                // memcpy(&clock_mgr.my_time, clock_mgr.time_p, sizeof(struct tm));
            }
            //clock_print_date_time(&t);
            clock_print_date_time(&clock_mgr.my_time);
        }
        else Serial.println("!!!msg_time_action: Integer conversion Error");
    }
    else {
        Serial.println("Incorrect Time message");
    }
    clock_mgr.next_minute = millis() + 60000;
}

#define CLOCK_BUFF_LEN  40
uint32_t timeout;
char    buff[CLOCK_BUFF_LEN];
void clock_task(void)
{

    switch(clock_th.state)
    {
        case 0:
            clock_mgr.ntp_is_running = false;
            clock_mgr.rtc_is_running = false;
            clock_th.state = 10;
            timeout = millis() + 5000;
            break;
        case 10:
            if(rtc_is_running()) {
                alpha_wr_4_char("RTC!",2000);
            }
            if(millis() > timeout) clock_th.state = 20;
            break;

        case 20:
            alpha_wr_4_char("NTP?",2000);            
            clock_th.state = 30;
            timeout = millis() + 30000;
            break;
        case 30:
            if(ping_is_running()){
                alpha_wr_4_char("NTP!",2000);
                clock_th.state = 50;
                clock_mgr.time_p = &clock_mgr.ntp_time;
                timeout = millis() + 3000;
                clock_mgr.prev_minute = 99;
            } 
            if(millis() > timeout) clock_th.state = 50;
            break;
        case 50:
            if(millis() > timeout) clock_th.state = 100;
            break;    
        case 100:
            // set time from ntp
            clock_mgr.ntp_is_running = ping_is_running();
            clock_mgr.rtc_is_running = rtc_is_running();

            if (clock_mgr.ntp_is_running){
                clock_mgr.my_time = clock_mgr.ntp_time;
            } 
            else if (clock_mgr.rtc_is_running){
                clock_mgr.my_time = clock_mgr.rtc_time;
            }

            if(clock_mgr.ntp_is_running || clock_mgr.rtc_is_running)
            {
                if(clock_mgr.prev_minute != clock_mgr.my_time.tm_min )
                {   
                    clock_mgr.prev_minute = clock_mgr.my_time.tm_min;
                    if(clock_mgr.ntp_is_running){
                        strftime(buff, sizeof(buff), "%H.%M", &clock_mgr.my_time);
                    } else {
                        strftime(buff, sizeof(buff), "%H%M.", &clock_mgr.my_time);
                    }
                    alpha_wr_4_char(buff,0);

                    strftime(buff, sizeof(buff), "<T;#;PING;%Y;%m;%d;%H;%M;", &clock_mgr.my_time);
                    uint8_t dark_level = clock_get_dark_level();
                    size_t len = strnlen(buff,sizeof(buff));
                    sprintf(&buff[len],"D;%d>", dark_level);
                    Serial.println(buff);    
                    Serial1.println(buff);    
                    r69_send(buff);

                }
            }
            else {
                alpha_wr_4_char("Err0",2000);
            }

            // clock_th.state = 10;
            break;


    }
    if (millis() > clock_mgr.next_minute)
    {
        clock_mgr.next_minute += 6000;
        clock_mgr.my_time.tm_min += 1;
        time_t t = mktime(&clock_mgr.my_time);  // normalize (handles overflow)
        clock_mgr.my_time = *localtime(&t);     // write back normalized result
        clock_print_date_time(&clock_mgr.my_time);

        //clock_mgr.next_minute += 60000;
    }
    if ( clock_mgr.last_hour != clock_mgr.my_time.tm_hour)
    {
        clock_mgr.last_hour = clock_mgr.my_time.tm_hour;
        Serial.printf("Hour: %d", clock_mgr.my_time.tm_hour);
    }
}
