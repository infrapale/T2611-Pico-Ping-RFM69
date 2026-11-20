#ifndef __PING_H__
#define __PING_H__

typedef struct
{
    uint16_t state;
    uint16_t prev_state;
    uint16_t retry_cntr;
    uint32_t next_try;
    uint32_t wait_until;
    uint8_t   task_indx;
    time_t   now;
    // struct tm timeinfo;
    char    buffer[64];
    uint32_t next_io_tick;
    bool     is_running;
} ping_st;



void ping_initialize(void);

bool ping_is_running(void);

void ping_print_main_data(void);

#endif