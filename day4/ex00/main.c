#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#include "src/lib.h"
#include "src/interrupt.h"

// LED D1 = PB0
// SW1 = PD2

// 0x0002 INT0_vect           External pin INT0 (PD2)            -> EIMSK[INT0]
void INT0_vect( void ) __attribute__ ((__signal__, __used__));
void INT0_vect( void )
{
    if (IS_BIT_OFF(EICRA, ISC00)) // falling mode → button pressed
    {
        TOOGLE_PIN(PORTB, PB0);
        SET_PIN(EICRA, ISC00); // rising mode: wait for release
    }
    else                          // rising mode → button released
        UNSET_PIN(EICRA, ISC00); // back to falling: wait for next press
    _delay_ms(20);
    EIFR = _BV(INTF0); // discard bounce edges (by writting a 1; p81)
}

/////////////////////////////////////////////////////////////////////////////]
void interupt_init( void ) 
{
// enable interrupt for EIMSK[INT0]: External pin PD2
    SET_INTERRUPT_ON(EIMSK, INT0);
// SW1 is in default 1 > pessing it makes it 0 > falling edge (1 → 0) mode
    SET_PIN(EICRA, ISC01);
    UNSET_PIN(EICRA, ISC00);
    
    ENABLE_INTERRUPT;
}

/*/////////////////////////////////////////////////////////////////////////////]
For this exercise and the following ones, you are not allowed to use
the ISR macro provided by the AVR library. But nothing is stopping
you from taking a look at it ;)

• You are required to write a program that changes the state of LED D1 when the
button SW1 is pressed.
• You must use interrupts to read the state of the button. Reading the PINx registers
is not allowed.                      
                                             */
/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    SET_OUTPUT_LED1;
    interupt_init();

    while (1) {}
}
