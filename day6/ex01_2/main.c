#include <util/delay.h>

#include "i2c.h"
#include "uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
• Complete the previous program by adding a function i2c_write which will write the
contents of the TWDR register of the microcontroller and send it to the temperature
sensor.
• You must write a function i2c_read which will display the contents of the TWDR
register after the sensor measurement.
• You must write a function print_hex_value which will write the contents of the
7 bytes of an AHT20 sensor measurement without modification to the standard
output of your PC.
• You must display the return values in a loop that repeats with a frequency that
meets the manufacturer’s recommendations.
*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    i2c_init();

    while (1)
    {
        // 1st loop: also covers the AHT20 power-on delay (>= 40ms)
        _delay_ms(AHT20_PERIOD_MS);

        // trigger a measurement
        i2c_start();
        i2c_write(START_WRITTING(AHT20_ADDR)); // SLA+W
        i2c_write(AHT20_CMD_TRIGGER);
        i2c_write(0x33);
        i2c_write(0x00);
        i2c_stop();

        // the sensor is measuring
        _delay_ms(AHT20_MEASURE_MS);

        // read the 7 bytes
        i2c_start();
        i2c_write(START_READING(AHT20_ADDR)); // SLA+R
        i2c_read();
        i2c_stop();
    }
}
