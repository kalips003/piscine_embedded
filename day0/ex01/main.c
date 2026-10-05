#include <avr/io.h>

int main( void ) {

// set ddrb[0-7] = input (0) or output (1)
    DDRB |= (1 << PB0); // 0b00000001
// set portb[0-7] = 0V - 5V
    PORTB |= (1 << PB0); // 0b00000001

    while (1)
    {
    }

}