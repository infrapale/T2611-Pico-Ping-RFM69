/******************************************************************************
    uart.cpp:  Serial2 handler
-------------------------------------------------------------------------------
    Currently not in use
-------------------------------------------------------------------------------

******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <Arduino.h>
#include "main.h"
#include "uart.h"
#include "msg.h"
#include "io.h"
#include "atask.h"


uart_st uart = {0};

void uart_rx_task(void);
//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st rx_th         =         { "UART RX        ", 100,    0,     0,  255,    0,  1,  uart_rx_task };


void uart_initialize(void)
{
    uart.tx.avail = true;
    uart.rx.avail = false;
    atask_add_new(&rx_th);

}

void uart_send(char *buff)
{
    strncpy(uart.tx.buff, buff, UART_BUFF_LEN);
    Serial2.println(uart.tx.buff);
    Serial.printf("->UART 2: %s\n",uart.tx.buff);
}

bool uart_rx_avail(void){
    return uart.rx.avail;
}

bool uart_read(char *buff){
    if(uart.rx.avail){
        uart.rx.avail = false;
        strncpy(buff, uart.rx.buff,UART_BUFF_LEN);
        return true;
    } else {
        return false;
    }
}

void uart_rx_task(void)
{
    static  char c;
    switch(rx_th.state)
    {
        case 0:
            rx_th.state = 5;
            break;
        case 5:
            uart.rx.pos = 0;
            uart.rx.avail = false;
            rx_th.state = 10;
            break;
        case 10:
            while (Serial2.available()) {
                c = Serial2.read();
                //Serial.printf("@%d-%c\n",rfm_pos,c);
                if (c == '\n' || c == '\r') {
                    if (uart.rx.pos > 0) {
                        uart.rx.buff[uart.rx.pos] = 0;
                        uart.rx.avail = true;   // signal to msg_task()
                        rx_th.state = 20;
                        //Serial.println(uart.rx.buff);
                    }
                } else {
                    if (uart.rx.pos < UART_BUFF_LEN - 1) {
                        uart.rx.buff[uart.rx.pos++] = c;
                    } else {
                        // overflow protection
                        uart.rx.buff[UART_BUFF_LEN - 1] = 0;
                        rx_th.state = 100;
                    }
                }
            }
            break;
        case 20:
            Serial.println(uart.rx.buff); Serial.flush();
            uart.rx.field_count = msg_split(uart.rx.buff, ';');
            //sensor_process_msg(uart.rx.field_count);
            rx_th.state = 5;
            break;
        case 100:
            uart.rx.buff[UART_BUFF_LEN - 1] = 0;
            Serial.print("RX buffer limit:");
            rx_th.state = 20;
            break;
        case 200:
            rx_th.state = 10;
            break;
    }

}