/*****************************************************************************
T2603_Picolina_Ping_RFM69 
*******************************************************************************

HW: Pi Pico W + RFM69

*******************************************************************************
https://github.com/infrapale/T2310_RFM69_TxRx
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
*******************************************************************************



*******************************************************************************
**/
#include    "Wire.h"
#include    "main.h"
#include    "secrets.h"
#include    "r69.h"
#include    "atask.h"
#include    "io.h"
#include    "ping.h"
#include    "eeprom.h"
#include    "comm.h"
#include    "app.h"
#include    "alpha.h"
#include    "super.h"

#define PIN_TX0             (0u)
#define PIN_RX0             (1u)

#define IO_TICK_INTERVAL    (100)
#define COMM_TICK_INTERVAL  (100)

main_ctrl_st ctrl = {0};
main_data_st main_data;
void modem_task(void);
void print_debug_task(void);

atask_st modem_handle              = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
atask_st debug_th                  = {"Debug Task     ", 2000,    0,     0,  255,    0,  1,  print_debug_task };


void setup() {
    Serial1.setTX(PIN_TX0);   
    Serial1.setRX(PIN_RX0);

    Serial.begin(9600);
    delay(2500);
    Serial1.begin(9600);
    Serial.println(__APP__);
    Wire1.setSDA(PIN_I2C1_SDA);
    Wire1.setSCL(PIN_I2C1_SCL);
    Wire1.begin();
    uint8_t key[] = RFM69_KEY;

    ctrl.app_indx = APP_ARBORETUM;
    atask_initialize();
    ping_initialize();
    // atask_add_new(&modem_handle);
    atask_add_new(&debug_th);
    switch((app_indx_et)ctrl.app_indx)
    {
        case APP_VILLA_ASTRID:
            r69_initialize();
            break;
        case APP_PORTIMO_POLKU:
            break;
        case APP_ARBORETUM:
            r69_initialize();
            break;
        default:
            break;
    }
    aplha_initialize();

    // comm_initialize();
    // eeprom_initialize();
    // eeprom_load_main_data();
    // main_data.restart_cntr++;
    // eeprom_save_main_data();
    // ping_print_main_data();

}

void setup1(){
    io_initialize();
    ctrl.next_io_tick = millis() + IO_TICK_INTERVAL;
    ctrl.next_comm_tick = millis() + COMM_TICK_INTERVAL;
    super_initialize();
}

void loop() 
{
    atask_run();
    super_clear_cntr(SUPER_CNTR_LOOP);
}

void loop1()
{
    if(millis() > ctrl.next_io_tick){
        ctrl.next_io_tick = millis() + IO_TICK_INTERVAL;
        io_task();
    }
    if(millis() > ctrl.next_comm_tick){
        ctrl.next_comm_tick = millis() + COMM_TICK_INTERVAL;
        comm_task();
    }
    if(millis() > ctrl.next_super_tick){
        ctrl.next_super_tick = millis() + SUPER_TASK_INTERVAL_MS;
        super_task();
    }
    super_clear_cntr(SUPER_CNTR_LOOP1);
}

void print_debug_task(void)
{
  atask_print_status(true);
}

