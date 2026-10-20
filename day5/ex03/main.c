#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "src/analog.h"
#include "src/lib.h"
#include "src/uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
• Read the internal temperature sensor value
• Then display it on the console and convert it to celcius every 20ms.

*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    ini_adc_temperature_reading();

    while (1) {
        uint16_t temp = translate_temperature(take_a_reading_of_temp());
        char t[7];
        char* p = itoa(temp, t);
        *p = '\0';
        uart_printstr(t);
        uart_printstr("\r\n");
        _delay_ms(20);
    }
}

