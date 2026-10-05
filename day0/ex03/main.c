#include <avr/io.h>
#include <util/delay.h>

// LED0 = PB0
// SW1 = PD2

// PD2 = 0 == button pressed
// PD2 = 1 == button not pressed
int main(void)
{

    DDRB |=  (1 << PB0);    // PB0 = output (LED D1)
    DDRD &= ~(1 << PD2);    // PD2 = input  (button SW1)
 
    int button_status = PIND & (1 << PD2);

    while (1) {

        int current = PIND & (1 << PD2);

        if (button_status && !current) {     // bit is 0, so pressed (active-low)

            PORTB ^=  (1 << PB0);     // LED toogle
        }

        button_status = PIND & (1 << PD2);

        _delay_ms(50);
    }
}
