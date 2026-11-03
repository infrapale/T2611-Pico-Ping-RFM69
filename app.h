#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>

#define APP_NAME_LEN                20
#define APP_MAX_TOTAL_NBR_OF_RUUVI  42
#define APP_BUFF_LEN                80

typedef enum 
{
    APP_UNDEFINED = 0,
    APP_VILLA_ASTRID ,
    APP_PORTIMO_POLKU,
    APP_ARBORETUM,
    APP_3,
    APP_4,
    APP_5,
    APP_6,
    APP_7,
    APP_NBR_OF
} app_indx_et;


#endif