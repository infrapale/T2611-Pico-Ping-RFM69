#ifndef __CLOCK_H__
#define __CLOCK_H__
#include "msg.h"

#define MONTHS_PER_YEAR 12

typedef enum
{
  JANUARY = 0,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
} month_et;


typedef struct 
{
    struct tm   ntp_time;
    struct tm   rtc_time;
    struct tm   my_time;
    struct tm   *time_p;
    char        buff[64];
    uint8_t     prev_minute;
    uint32_t    next_minute;
    uint8_t     last_hour;
    bool        ntp_is_running;
    bool        rtc_is_running;
    bool        new_hour_event;
    bool        new_minute_event;
} clock_st;

void clock_initialize(void);

uint8_t clock_get_dark_level(void);

void clock_set_date_time( msg_st *msgp);


#endif