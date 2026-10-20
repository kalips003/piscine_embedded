#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "src/analog.h"
#include "src/lib.h"
#include "src/uart.h"

void take_all_reading(char* print)
{
    print = itoa(take_a_reading(0), print);
    *(print) = ',';
    *(++print) = ' ';
    print = itoa(take_a_reading(1), ++print);
    *(print) = ',';
    *(++print) = ' ';
    print = itoa(take_a_reading(2), ++print);
    *(print) = '\r';
    *(++print) = '\n';
    *(++print) = '\0';
}

/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
Read the potentiometer RV1 + the LDR (R14) + the NTC (R20)
But this time, you must use the ADC in 10bit mode
• After that, you must display the values in hexadecimal format every 20ms on the
console.
ADC_POT PC0(ADC0)
ADC_LDR PC1(ADC1)
ADC_NTC PC2(ADC2)

> 0, 128, 1023
*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    SET_READING_PIN(DDRC, PC0); // set PC0 as reading
    uart_init();
    ini_adc();

    char buff[19] = {0};

    while (1) {
        take_all_reading(buff);
        uart_printstr(buff);
        _delay_ms(20);
    } 
}

