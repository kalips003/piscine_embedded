#include <avr/io.h>


#define SET_WRITING_PIN(register, pin) (register |= _BV(pin))
#define SET_READING_PIN(register, pin) (register &= ~_BV(pin))
#define READ_PIN(register, pin) ((register & _BV(pin)) != 0)
#define TURN_ON_PIN(register, pin) (register &= _BV(pin))
#define TURN_OFF_PIN(register, pin) (register &= _BV(pin))

// LED1 on PB0
#define SET_WRITING_PIN_B0 SET_WRITING_PIN(DDRB, PB0) 
// button SW1 on PD2
#define SET_READING_PIN_D2 SET_READING_PIN(DDRD, PD2) 

// SW1 is active-low: idle = 1, pressed = 0
#define SW1_NOT_PRESSED READ_PIN(PIND, PD2)
#define SW1_PRESSED !READ_PIN(PIND, PD2)
#define TURN_ON_LED1 TURN_ON_PIN(PORTB, PB0)
#define TURN_OFF_LED1 TURN_OFF_PIN(PORTB, PB0)

/////////////////////////////////////////////////////////////////////////////]
int main(void)
{
// LED1 on PB0
    SET_WRITING_PIN_B0;
// button SW1 on PD2
    SET_READING_PIN_D2;

    while (1) {
        if (SW1_PRESSED)     // bit is 0, so pressed (active-low)
            TURN_ON_LED1;     // LED on
        else
            TURN_OFF_LED1;     // LED off
    }
}