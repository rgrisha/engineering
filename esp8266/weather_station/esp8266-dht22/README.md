# esp8266-dht22
A DHT library for the ESP8266

This library builds on top of the ESP8266 SDK to support the DHTXX family of humidity and temperature sensors.  
Supported models are:
-DHT11
-DHT21 (AM2301) 
-DHT22 (AM2302)

It is a port of the DHT Sensor library for the arduino created by @adafruit , which you can find [here](https://github.com/adafruit/DHT-sensor-library).

Additional information about the DTHXX can be found in this [spec sheet](https://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf).

## Usage
The library is comprised of a single header and source code file.  Simply include dht.h at the top of your program to get started.

There are only 4 functions you'll need to read temperature and humidity data.
```C
/*Initializes the DHT.  User must specify which GPIO on the ESP8266 is connected to the DHT and which member of the DHTXX family is being used.  Options are DHT11, DHT21, AM2301, DHT22, AM2302.  This must be called before reading the DHT*/
void DHT_init(uint8_t pin, uint8_t type);

/*Prepare ESP8266 for reading the DHT.  This must be called before reading the DHT.*/
void DHT_begin(void);

/*Returns a floating point value of the temperature, or -1 if failed.  User can specify if they want value in Celsius or Fahrenheight*/
float readTemperature(bool isFahrenheit);

/*Returns a floating point value of the humidity, or -1 if failed.*/
float readHumidity();
```

A a quick example, here is a sample user_main.c file which prints out the humidity and temperature every 3 seconds.
```C
#include "dht22.h"
#define baud_rate 115200

volatile os_timer_t read_humidity_timer;
void read_humidity_timerfunc(void);

void user_init(void)
{
        uart_div_modify(0, UART_CLK_FREQ / baud_rate); 
        os_delay_us(10);

        //Setup a timer to read DHT every 3 seconds, on repeat.
        os_timer_setfn(&read_humidity_timer, (os_timer_func_t *)read_humidity_timerfunc, NULL);
        os_timer_arm(&read_humidity_timer, 3000, 1);

        //Declare we are using a DHT22 on Pin 14
        DHT_init(14,DHT22);
        DHT_begin();
}


void read_humidity_timerfunc(void)
{
        float t = readTemperature(false);
        float h = readHumidity();
        
        os_printf("Temperature (C): %d\r\n", (int)t);
        os_printf("Humidity (C): %d\r\n", (int)h);
}
```

##What's been tested
-DHT22 with an Adafruit Huzzah

##Known issues
The 1 wire communication bus relays on very exact timing.  During the signal reading process any interrupts will corrupt the result.  In Adafruit's Arduino implementation they disable all interrupts during the reading process.  The ESP8266 cannot disable all interrupts so if an interrupt does occur expect a bad result.  Luckily, bad results are returned as -1, so you have a way to verify.

On the Adafruit Huzzah using certain pins will result in failure to read the signal if you don't power cycle your ESP8266 after flashing and after hitting the reset button.  So far, this has happened on Pins #2 and #4.  Pin #14, however, does not have this issue.  When in doubt, power cycle!

##Contact the author
Reach out on twitter [@iotpipe](https://twitter.com/iot_pipe)
