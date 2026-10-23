#include <stdint.h>
#include <util/delay.h>

#include "src/i2c.h"
#include "src/uart.h"
#include "src/tools.h"

void    AHT20_read_one_mesurement(char* buff);

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
        i2c_start();
        i2c_stop();
        uart_printstr(" ---\r\n");
        _delay_ms(500);
    }
}

int main2( void )
{
    uart_init();
    i2c_init();

    char buffer[23] = {0};
    while (1)
    {
        AHT20_read_one_mesurement(buffer);
        uart_printstr(buffer);
        // uart_printstr("are you seeing this?\r\n");
        _delay_ms(500);
    }

}

void    AHT20_read_one_mesurement(char* buff)
{
    uint8_t data_part;

    // new conversation: START + SLA+R (0 = read)
    if (!i2c_start_better() || !i2c_start_transmit(AHT20_ADDR, 0)) {
        i2c_stop();
        return;
    }

    for (uint8_t i = 0; i < 7; ++i) {
        
        // if (!i2c_read(0, &data_part)) {
        //     char hex[3] = {0};
        //     write_hexa(I2C_STATUS, hex);
        //     uart_printstr("read err: ");
        //     uart_printstr(hex);
        //     uart_printstr("\r\n");
        //     return;
        // }

        // ACK the first 6, NACK the 7th (CRC) = "that's enough"
        if (!i2c_read_better(i == 6, &data_part)) {
            i2c_stop();
            return; // some error whle in connection
        }

        // if (i == 0 && data_part == 0)
        //     return ; // still reading?
        write_hexa(data_part, buff);
        buff += 2;

        // if (i == 9 /* && data_part != CRC */)
        //     return ; // data not intact
        *buff++ = ' ';
    }
    // end of conversation, bus free again
    i2c_stop();

    *(--buff) = '\r';
    *(++buff) = '\n';
}
/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES

*//////////////////////////////////////////////////////////////////////////////]