
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#include <Wire.h>
#include "main.h"
#include "io.h"
#include "atask.h"

#define  ALPHA_BUFF_LEN     8

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

typedef struct 
{
    char new_buff[ALPHA_BUFF_LEN];
    char fb_buff[ALPHA_BUFF_LEN];
    uint32_t fb_timeout;
} alpha_ctrl_st;

void alpha_task(void);
//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st alpha_th         =      { "Alpha          ", 100,    0,     0,  255,    0,  1,  alpha_task };

alpha_ctrl_st alpha = {0};


void aplha_initialize(void) 
{
    // Wire1.setSDA(PIN_I2C1_SDA);   // GP2 = SDA
    // Wire1.setSCL(PIN_I2C1_SDA);   // GP3 = SCL

    alpha4.begin(0x70, &Wire1);  
    alpha4.writeDigitRaw(3, 0x0);
    alpha4.writeDigitRaw(0, 0xFFFF);
    alpha4.writeDisplay();
    delay(200);
    alpha4.writeDigitRaw(0, 0x0);
    alpha4.writeDigitRaw(1, 0xFFFF);
    alpha4.writeDisplay();
    delay(200);
    alpha4.writeDigitRaw(1, 0x0);
    alpha4.writeDigitRaw(2, 0xFFFF);
    alpha4.writeDisplay();
    delay(200);
    alpha4.writeDigitRaw(2, 0x0);
    alpha4.writeDigitRaw(3, 0xFFFF);
    alpha4.writeDisplay();
    delay(200);

    alpha4.clear();
    alpha4.writeDisplay();

    atask_add_new(&alpha_th);

  // display every character, 
    // for (uint8_t i='!'; i<='z'; i++) {
    //   alpha4.writeDigitAscii(0, i);
    //   alpha4.writeDigitAscii(1, i+1);
    //   alpha4.writeDigitAscii(2, i+2);
    //   alpha4.writeDigitAscii(3, i+3);
    //   alpha4.writeDisplay();
      
    //   delay(300);
    // }
    // Serial.println("Start typing to display!");
}


// #include "Adafruit_LEDBackpack.h"
void alpha_wr_4_char(const char *buff, uint32_t fb_timeout)
{
    uint8_t disp = 0;
    uint8_t src  = 0;

    alpha.fb_timeout = fb_timeout;
    if(fb_timeout == 0) {
        strncpy(alpha.fb_buff, buff, sizeof(alpha.fb_buff));
    }

    while (buff[src] != '\0' && disp < 4) {

        bool dp = false;

        // If next character is '.', attach it to this digit
        if (buff[src + 1] == '.') {
            dp = true;
        }

        alpha4.writeDigitAscii(disp, buff[src], dp);

        disp++;
        src++;

        // Skip over decimal point since it's now attached
        if (buff[src] == '.') {
            src++;
        }
    }

    // Blank any remaining display positions
    while (disp < 4) {
        alpha4.writeDigitAscii(disp++, ' ');
    }

    alpha4.writeDisplay();
}
// void alpha_wr_4_char(char *buff)
// {
//     for (uint8_t i=0; i <= 3; i++) {
//         alpha4.writeDigitAscii(i, buff[i]);
//     }
//     alpha4.writeDisplay();
// }
uint8_t alpha_map_brightness(uint16_t val)
{
    if (val > 2800) return 0;
    else if (val < 300) return 15;
    else  return (uint8_t) map(val,300,2800,15,0);
}

uint16_t ldr;
uint8_t inten;

void alpha_task(void)
{
    static uint32_t timeout;

    switch(alpha_th.state)
    {
        case 0:
            alpha_th.state = 10;
            alpha4.setDisplayState(false);
            break;
        case 10:
            if(io_get_pir()){
                timeout = millis() + 60000;
                alpha4.setDisplayState(true);
                alpha_th.state = 20;
            } else {
                alpha4.setDisplayState(true);
                alpha4.setBrightness(0);
            }
            if (alpha.fb_timeout != 0){
                if (millis() > alpha.fb_timeout) {
                    alpha_wr_4_char( alpha.fb_buff, 0);
                }
            }
            break;
        case 20:
            if(millis() > timeout) alpha_th.state = 10;
            ldr = io_get_ldr();
            alpha4.setDisplayState(true);
            inten = alpha_map_brightness(ldr);
            alpha4.setBrightness(alpha_map_brightness(ldr));
            break;
        case 30:
            alpha_th.state = 10;
            break;
        case 40:
            alpha_th.state = 10;
            break;
    }
}
