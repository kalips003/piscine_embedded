#include <avr/io.h>

// int main(void)
// {
//     DDRB = 0b00000001;        // PB0 outputs (1)

//     DDRD = 0b00000000;        // PD2 = input (0)

//     while (1) {

//         if ( (PIND & 0b100) == 0 ) // button is pressed
//             PORTB = 0b1;
//         // else if ( PIND & 0b100 == 1 ) // button not pressed
//         else
//             PORTB = 0b0;
    
//     }
// }

int main(void)
{
    DDRB |=  (1 << PB0);    // PB0 = output (LED D1)
    DDRD &= ~(1 << PD2);    // PD2 = input  (button SW1)

    while (1) {
        if (!(PIND & (1 << PD2)))     // bit is 0, so pressed (active-low)
            PORTB |=  (1 << PB0);     // LED on
        else
            PORTB &= ~(1 << PB0);     // LED off
    }
}