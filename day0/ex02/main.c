#include <avr/io.h>

int main(void)
{
//  read mode
    // writting
    DDRB |=  (1 << PB0);    // PB0 = output (LED D1)
    // reading
    DDRD &= ~(1 << PD2);    // PD2 = input  (button SW1)

    while (1) {
        if (!(PIND & (1 << PD2)))     // bit is 0, so pressed (active-low)
            PORTB |=  (1 << PB0);     // LED on
        else
            PORTB &= ~(1 << PB0);     // LED off
    }
}