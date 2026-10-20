#include "analog.h"

#include <stdint.h>
#include <util/delay.h>

/*/////////////////////////////////////////////////////////////////////////////]
                                TEMPERATURE
*//////////////////////////////////////////////////////////////////////////////]
void ini_adc_temperature_reading()
{
    ADC_SET_PRESCALER(1,1,1); // prescaler 128
    ADC_SET_REF_SELECTION(1,1); // internal 1.1V, with a capacitor on AREF for temp reading
    SET_READING_10BITS;
    ADC_SET_ANALOG_CHANNEL_SELECTION(1,0,0,0); // select temp sensor

    ADC_ENABLE;
    _delay_ms(10); // wait for setup before first reading
}

uint16_t take_a_reading_of_temp()
{
    ADC_START;
    ADC_WAIT_RESULT;
    return ADC; // read 10 bits
}

/*/////////////////////////////////////////////////////////////////////////////]
*//////////////////////////////////////////////////////////////////////////////]
// Temperature/°C   –40°C   +25°C   +125°C
//                  0x010D  0x0160  0x01E0
//                  269     352     480
// > T(°C) = (ADC - 352) / 1.28 + 25
int16_t translate_temperature(int adc)
{
    int16_t t = ((int16_t)adc - 352) * 100 / 128 + 25;
    return t;
}

//////////////////////////////////////////////////////////////////////////////]

uint16_t take_a_reading(uint8_t what_rc)
{
    ADMUX = (ADMUX & ~0b1111) | what_rc;
    ADC_START;
    ADC_WAIT_RESULT;
    return ADC; // read 10 bits
}
