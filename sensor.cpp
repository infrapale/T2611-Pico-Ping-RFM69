#include <Wire.h>
// #include <AHT20.h>
#include <Adafruit_AHTX0.h>
#include "sensor.h"
#include "main.h"
#include "io.h"
#include "atask.h"
#include "r69.h"
#include "alpha.h"

#define BUFF_LEN  40

typedef struct
{
    bool  status;
    bool  updated;
    float temperature;
    float humidity;
    uint32_t next_read;
    uint32_t next_send;
    char  buff[BUFF_LEN];
} sensor_st;

Adafruit_AHTX0 aht20;

void sensor_task(void);
//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st sensor_th         =     { "Sensor Task    ", 1000,   0,     0,  255,    0,  1,  sensor_task };

sensor_st  sensor = {0};

void sensor_initialize(void)
{
    // sensor.status = aht20.begin(0x70, &Wire);


    Wire.begin();
    sensor.status = aht20.begin(&Wire1, 0, 0x38);
    if(sensor.status) {
        Serial.printf("AHT20 - Bueno");
        atask_add_new(&sensor_th);
    } else {
        Serial.printf("AHT20 - Kein sensor gefunden");
    }

}

void sensor_task(void)
{
    switch(sensor_th.state)
    {
        case 0:
            if(sensor.status) sensor_th.state = 10;
            break;
        case 10:
            sensor.next_read = millis() + 2000;
            sensor.next_send = millis() + 10000;            
            sensor_th.state = 20;
            break;
        case 20:
            if(millis() > sensor.next_read) sensor_th.state = 30;
            if(millis() > sensor.next_send) sensor_th.state = 100;
            break;
        case 30:
            sensors_event_t humidity, temp;
            aht20.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
            sensor.temperature = temp.temperature;
            sensor.humidity = humidity.relative_humidity;
            sensor.updated = true;
            sensor.next_read = millis() + 10000;
            sensor_th.state = 20;
            break;
        case 100:
            sprintf(sensor.buff, "<S;Ping;T;%0.1f;H;%0.0f>",
                sensor.temperature,
                sensor.humidity);
            Serial.println(sensor.buff);
            r69_send(sensor.buff);
            if(sensor.temperature >= 0) sprintf(sensor.buff, "+%0.1f", sensor.temperature);
            else sprintf(sensor.buff, "%0.1f");
            Serial.println(sensor.buff);    
            alpha_wr_4_char(sensor.buff, 5000);        
            sensor.next_send = millis() + 60000;
            sensor_th.state = 20;
            break;
    }
}


//   aht20.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
//   Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
//   Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
