#include <avr/io.h>

#define HALF_SENCOND 400000

void delay(void)
{
    for (volatile long i = 0; i < HALF_SENCOND; i++)
        ;
}

// LED0 = PB0.pdf
// SW1 = PD2

// PD2 = 0 == button pressed
// PD2 = 1 == button not pressed

/* 
◦ increments a value each time you press button SW1
◦ decrements a value each time you press button SW2
◦ displays this value in binary on LEDs D1 D2 D3 and D4 permanently
*/
int main(void)
{
    DDRB |= (1 << PB1);    // PB0 = output (LED D1-D4)

    while (1) {

        PORTB ^= (1 << PB1);

        delay();
    }
}