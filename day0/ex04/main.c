#include <avr/io.h>
#include <util/delay.h>

// LED0 = PB0
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
    DDRB |=  (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);    // PB0 = output (LED D1-D4)
    DDRD &= ~((1 << PD2) | (1 << PD4));    // PD2 = input  (button SW1 - SW2)
 
    uint8_t count = 0;
    int button1_idle = PIND & (1 << PD2);
    int button2_idle = PIND & (1 << PD4);

    PORTB = bin_to_board(count);
    while (1) {

        int button1_pressed = !(PIND & (1 << PD2));
        int button2_pressed = !(PIND & (1 << PD4));

        if (button1_idle && button1_pressed) { // increment

            count++;
            PORTB = bin_to_board(count);
        }
        if (button2_idle && button2_pressed) { // decrement

            count--;
            PORTB = bin_to_board(count);
        }

        button1_idle = PIND & (1 << PD2);
        button2_idle = PIND & (1 << PD4);
        _delay_ms(50);
    }
}

// 0b00001111 > 0b00010111
uint8_t bin_to_board(uint8_t num) {

    return (num & 0b00000111) | ((num & 0b00001000) << 1);
}