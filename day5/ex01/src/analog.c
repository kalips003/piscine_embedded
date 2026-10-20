#include "analog.h"

#include <stdint.h>
#include <util/delay.h>

void ini_adc()
{
    ADC_SET_PRESCALER(1,0,1); // prescaler 32
    ADC_SET_REF_SELECTION(0,1); // avcc 5V, with a capacitor on AREF
    SET_READING_8BITS;
    ADC_SET_ANALOG_CHANNEL_SELECTION(0,0,0,0);

    ADC_ENABLE;
    _delay_ms(10); // wait for setup before first reading
}

uint8_t take_a_reading(uint8_t what_rc)
{
    ADMUX = (ADMUX & ~0b1111) | what_rc;
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
