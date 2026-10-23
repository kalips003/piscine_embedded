#include <stdlib.h> // dtostrf
#include <util/delay.h>

#include "i2c.h"
#include "uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
• Complete the previous program to display the temperature and humidity in place
of the raw values obtained earlier.
• The temperature and humidity values should be displayed according to the following
specifications:
    ◦ the precision displayed on the terminal must respect the recommendations
    given in the datasheet, rounded to the nearest ten.
    (e.g.: 0.0031 -> 0.01 or 0.1 -> 0.1)
    ◦ the temperature will be in degrees Celsius (ºC), the humidity in percent (%).
    ◦ the displayed measurement must be the average of the last 3 measurements
    taken by the program. The behavior for the first two measurements should be
    consistent with this rule.
*//////////////////////////////////////////////////////////////////////////////]
#define AVG_COUNT 3

// one measurement: trigger, wait, read the 7 bytes
static void aht20_measure(uint8_t raw[7])
{
    i2c_start();
    i2c_write(START_WRITTING(AHT20_ADDR)); // SLA+W
    i2c_write(AHT20_CMD_TRIGGER);
    i2c_write(0x33);
    i2c_write(0x00);
    i2c_stop();

    // the sensor is measuring
    _delay_ms(AHT20_MEASURE_MS);

    i2c_start();
    i2c_write(START_READING(AHT20_ADDR)); // SLA+R
    for (uint8_t i = 0; i < 7; ++i)
        raw[i] = i2c_read_byte(i == 6); // NACK the 7th
    i2c_stop();
}

// Temperature: 17.C, Humidity: 43,4%
static void print_values(float temp, float hum)
{
    char buf[12]; // sign + 3 digits + '.' + decimals + '\0', with room to spare

    uart_printstr("Temperature: ");
    dtostrf(temp, 0, 2, buf); // resolution 0.01C -> 2 decimals
    uart_printstr(buf);
    uart_printstr("\xC2\xB0""C, Humidity: "); // UTF-8 '°'
    dtostrf(hum, 0, 1, buf); // resolution 0.024% -> 0.1% -> 1 decimal
    uart_printstr(buf);
    uart_printstr("%\r\n");
}

int main( void )
{
    uart_init();
    i2c_init();

    uint8_t raw[7];
    float   temps[AVG_COUNT];
    float   hums[AVG_COUNT];
    uint8_t idx = 0;
    uint8_t count = 0; // 1, 2, then stays at 3: average of what we have so far

    while (1)
    {
        // 1st loop: also covers the AHT20 power-on delay (>= 40ms)
        _delay_ms(AHT20_PERIOD_MS);

        aht20_measure(raw);

        uint32_t s_rh = (uint32_t)raw[1] << 12 | (uint32_t)raw[2] << 4 | raw[3] >> 4;
        uint32_t s_t  = ((uint32_t)raw[3] & 0x0F) << 16 | (uint32_t)raw[4] << 8 | raw[5];

        hums[idx]  = (float)s_rh / 1048576.0 * 100.0;          // %RH
        temps[idx] = (float)s_t  / 1048576.0 * 200.0 - 50.0;   // °C
        idx = (idx + 1) % AVG_COUNT;
        if (count < AVG_COUNT)
            count++;

        float temp = 0, hum = 0;
        for (uint8_t i = 0; i < count; ++i) {
            temp += temps[i];
            hum  += hums[i];
        }
        print_values(temp / count, hum / count);
    }
}
