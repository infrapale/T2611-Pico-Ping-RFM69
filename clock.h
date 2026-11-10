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
    struct tm   my_time;
    char        buff[64];
    uint32_t    next_minute;
    uint8_t     last_hour;
    bool        new_hour_event;
} clock_st;

void clock_initialize(void);

uint8_t clock_get_dark_level(void);

void clock_set_date_time( msg_st *msgp);


#endif