#ifndef __RTC_H__
#define __RTC_H__

void rtc_initialize(void);

bool rtc_is_running(void);

void rtc_copy_time( struct tm *time);

#endif