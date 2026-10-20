#include <avr/io.h>

// 0b00001111 > 0b00010111
// bit shift the 4th bit
#define MOVE_4TH_BIT(num) ((num & 0b00000111) | ((num & 0b00001000) << 1))

#include "src/interrupt.h"
#include "src/lib.h"
#include <util/delay.h>

volatile uint8_t num = 0;

// 0x0002 INT0_vect           External pin INT0 (PD2)            -> EIMSK[INT0]
void INT0_vect( void ) __attribute__ ((__signal__, __used__));
void INT0_vect( void )
{
    if (IS_BIT_OFF(EICRA, ISC00)) // falling mode → button pressed
    {
        if (num < 15)
            num++;
        PORTB = MOVE_4TH_BIT(num); // display LEDs
        SET_PIN(EICRA, ISC00); // rising mode: wait for release
    }
    else                          // rising mode → button released
        UNSET_PIN(EICRA, ISC00); // back to falling: wait for next press
    _delay_ms(20);
    EIFR = _BV(INTF0); // discard bounce edges (by writting a 1; p81)
}

//  0x000A PCINT2_vect         Pin Change group 2 (PORTD)         -> PCICR[PCIE2]
void PCINT2_vect( void ) __attribute__ ((__signal__, __used__));
void PCINT2_vect( void )
{
    UNSET_PIN(PCICR, PCIE2); // discard futur interrupt
    if (SW2_PRESSED)
    {
        if (num > 0)
            num--;
        PORTB = MOVE_4TH_BIT(num); // display LEDs
    
    }
    _delay_ms(20);
    SET_PIN(PCICR, PCIE2);
}

void interrupt_init()
{
    SET_PIN(EIMSK, INT0); // ENABLE INTERRUPT ON PD2(INT0)
    SET_PIN(EICRA, ISC01); // INTERRUPT ON RAISING EDGE
    UNSET_PIN(EICRA, ISC00);

    SET_PIN(PCICR, PCIE2); // enable interrupt for the mask PD)
    SET_PIN(PCMSK2, PCINT20); // enable the interrupt only on PD4

    ENABLE_INTERRUPT;
}



// PD2 = 0 == button pressed
// PD2 = 1 == button not pressed
// SW1 = PD2; SW2 = PD4
// LEDS[1:4] = PD0 + PD1 + PD2 + PD4
int main(void)
{
    // enable 4 leds
    DDRB |=  (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);    // PB0 = output (LED D1-D4)
    // enable the sw in reading?
    interrupt_init();

    while (1) {}
}