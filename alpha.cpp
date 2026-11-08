
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#include <Wire.h>
#include "main.h"
#include "io.h"
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

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

void alpha_wr_4_char(char *buff)
{
    for (uint8_t i=0; i <= 3; i++) {
        alpha4.writeDigitAscii(i, buff[i]);
    }
    alpha4.writeDisplay();
}
