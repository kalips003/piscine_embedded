#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "src/analog.h"
#include "src/lib.h"
#include "src/uart.h"

void ini_adc()
{
    ADC_SET_PRESCALER(1,0,1); // prescaler 32
    ADC_SET_REF_SELECTION(0,1); // avcc 5V, with a capacitor on AREF
    SET_READING_8BITS;
    ADC_SET_ANALOG_CHANNEL_SELECTION(0,0,0,0);

    ADC_ENABLE;
    _delay_ms(10); // wait for setup before first reading
}

uint8_t take_a_reading()
{
    ADC_START;
    ADC_WAIT_RESULT;
    return ADCH; // read only 8 bits
}

static const char hex_base[] = "0123456789abcdef";

void write_hexa(uint8_t num, char buff[2])
{
    buff[0] = hex_base[(num & 0xF0) >> 4];
    buff[1] = hex_base[num & 0x0F];
}

void send_reading(char buff[2], uint8_t value)
{
    write_hexa(value, buff);
    uart_printstr(buff);
}

/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
Read the value of the linear potentiometer RV1 using the ADC peripheral.
• ADC must be configured with an 8-bit resolution and AVCC as a reference.
• Then display its value in hexadecimal format every 20ms on the console
*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    SET_READING_PIN(DDRC, PC0); // set PC0 as reading
    uart_init();
    ini_adc();

    char buff[5] = {0};
    buff[2] = '\r';
    buff[3] = '\n';

    while (1) {
        send_reading(buff, take_a_reading());
        _delay_ms(20);
    } 
}

