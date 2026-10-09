#include <avr/io.h>

#define HALF_SENCOND 400000

void delay(void)
{
    for (volatile long i = 0; i < HALF_SENCOND; i++)
        ;
}

int main(void)
{
    DDRB |= (1 << PB1);    // PB1 = output (LED D1-D4);

    while (1) {

        PORTB ^= (1 << PB1);    // PB1 = 5V;

        delay();
    }
}