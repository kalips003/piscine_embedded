#include <avr/io.h>
#include <util/delay.h>

// LED0 = PB0
// SW1 = PD2

// PD2 = 0 == button pressed
// PD2 = 1 == button not pressed
int main(void)
{
//  set write / read
    DDRB |=  (1 << PB0);    // PB0 = output (LED D1)
    DDRD &= ~(1 << PD2);    // PD2 = input  (button SW1)
 
//  current state of the button
    int button_status = PIND & (1 << PD2);

    while (1) {

        int current = PIND & (1 << PD2);

        if (button_status && !current) {     // previously not pressed

            PORTB ^=  (1 << PB0);     // LED toogle
        }

        // update the button status
        button_status = PIND & (1 << PD2);

        // wait for bounce effect
        _delay_ms(50);
    }
}
