#include "analog.h"

#include <stdint.h>
#include <util/delay.h>

void ini_adc()
{
    ADC_SET_PRESCALER(1,0,1); // prescaler 32
    ADC_SET_REF_SELECTION(0,1); // avcc 5V, with a capacitor on AREF
    SET_READING_10BITS;
    ADC_SET_ANALOG_CHANNEL_SELECTION(0,0,0,0);

    ADC_ENABLE;
    _delay_ms(10); // wait for setup before first reading
}

uint16_t take_a_reading(uint8_t what_rc)
{
    ADMUX = (ADMUX & ~0b1111) | what_rc;
    ADC_START;
    ADC_WAIT_RESULT;
    return ADC; // read 10 bits
}

static const char hex_base[] = "0123456789abcdef";
static const char dec_base[] = "0123456789";

void write_hexa(uint8_t num, char buff[2])
{
    buff[0] = hex_base[(num & 0xF0) >> 4];
    buff[1] = hex_base[num & 0x0F];
}

char* itoa(uint16_t num, char* buff)
{
    uint8_t len = 1;
    uint16_t tmp = num;

    while (tmp >= 10) {
        tmp /= 10;
        len++;
    }

    tmp = len;
    do {
        buff[--len] = dec_base[num % 10];
        num /= 10;
    } while (num != 0);

    return buff + tmp;
}