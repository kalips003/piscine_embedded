#include <avr/io.h>

int main( void ) {

// set ddrb[0-7] = input (0) or output (1)
    // LED D1 = PB0
    DDRB |= (1 << PB0);
// set  writting portb[0-7] = 0V - 5V
    PORTB |= (1 << PB0);

    while (1)
    {
    }

}