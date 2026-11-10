#include <Arduino.h>
#include <btstack.h>
#include "main.h"
#include "app.h"
#include "uart.h"
#include "io.h"
#include "atask.h"
// #include "r69.h"
// #include "msg.h"

extern main_ctrl_st main_ctrl;
// extern msg_st msg;


app_st app[APP_NBR_OF] =
{
    [APP_UNDEFINED] = {.name = "Undefined"},
    [APP_VILLA_ASTRID] = {.name = "VillaAstrid"},
    [APP_PORTIMO_POLKU] = {.name = "Portimopolku"},
    [APP_ARBORETUM] = {.name = "Arboretum"},
    [APP_3] = {.name = "App3"},
    [APP_4] = {.name = "App4"},
    [APP_5] = {.name = "App5"},
    [APP_6] = {.name = "App6"},
    [APP_7] = {.name = "App7"}
};


extern main_ctrl_st ctrl;
app_ctrl_st app_ctrl;


// void app_task(void);
// //                                  123456789012345   ival  next  state  prev  cntr flag  call backup
// atask_st app_th            =     { "App Task       ", 100,    0,     0,  255,    0,  1,  app_task };


void app_initialize(app_indx_et app_indx)
{
    app_ctrl.active = app_indx;
    ctrl.app_indx = app_indx;
    // atask_add_new(&app_th);
    app_print_summary();
}

void app_send_conf(void)
{
    // sprintf(app_ctrl.buff,"<C;%s;I;%d;V;%d;W;%d;L;%d>",
    //     app[app_ctrl.active].name,
    //     app_ctrl.active,
    //     __VER__,
    //     io_wd_is_enabled(),
    //     io_lte_is_enabled());
    // uart_tft_send(app_ctrl.buff);    
}


void app_print_summary(void)
{
    Serial.printf("Active app: %s\n", 
        app[app_ctrl.active].name);
}

void app_task(void)
{
    // static uint32_t timeout;
    // static uint8_t start_up_send_cntr = 0;

    // switch(app_th.state)
    // {
    //     case 0: 
    //         timeout = millis() + 2000;
    //         app_th.state = 10;
    //         break;
    //     case 10: 
    //         if(millis() > timeout){
    //             app_send_conf();
    //             if(++start_up_send_cntr > 4) {
    //                 app_th.state = 100;
    //             } else {
    //                 timeout = millis() + 2000;
    //             }
    //         } 
    //         break;
    //     case 100: 
    //         timeout = millis() + 60000;
    //         app_th.state = 110;
    //         break;
    //     case 110:
    //         if(millis() > timeout){
    //             app_send_conf();
    //             app_th.state = 100;
    //         }
    // }
}

