#include <util/delay.h>

#include "src/i2c.h"
#include "src/uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
• The AVR ATmega328P microcontroller has 1 I2C peripheral that you must use in
this exercise to communicate with an AHT20 temperature sensor (U3).
• You must write a function i2c_init that initializes the I2C on the microcontroller.
• The MCU’s I2C must be configured so that the communication frequency is 100kHz.
• You must write a function _better that starts an I2C transmission between the
microcontroller and the sensor.
• The program must return status values to your computer after each data transmis-
sion.
• You must write a function i2c_stop that interrupts communication between the
microcontroller and the sensor
*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    i2c_init();

    while (1)
    {
        // call slave
        i2c_start(); // 08
        i2c_write((AHT20_ADDR << 1) | 0); // SLA+W
        i2c_write(0xAC); // 3 bytes trigger
        i2c_write(0x33);
        i2c_write(0x00);
        i2c_stop();
        _delay_ms(80);

        // read
        i2c_start();
        i2c_write((AHT20_ADDR << 1) | 1); // SLA+R
        aht20_readings();
        i2c_stop();

        _delay_ms(2000);
    }
}
/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
byte:    [0]        [1]        [2]        [3]         [4]        [5]        [6]
       SSSSSSSS | HHHHHHHH   HHHHHHHH   HHHH TTTT | TTTTTTTT   TTTTTTTT | CCCCCCCC
        status  |  hum 19:12  hum 11:4   hum 3:0    temp 15:8  temp 7:0 |   CRC
                |                        temp 19:16                       |
                |←──── humidity: 20 bits ────→|←──── temperature: 20 bits ─→|


humidity: 0 → 0 %, 2^20 → 100 %
temperature: 0 → -50 °C, 2^20 → +150 °C (a 200 °C span starting at -50)
*//////////////////////////////////////////////////////////////////////////////]