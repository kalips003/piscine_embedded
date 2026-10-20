#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>

#include "src/lib.h"
#include "src/interrupt.h"
#include "src/timer.h"

// timer0[0:255]:
// 200 steps for 16mHz = 80000 Hz per step 
// 80000 / 1024 ~78.125
void init_timer()
{
// TIMER1:
    OCR1A = 0; // useless, but setting it by default 
    T1_SET_WGM(0,1,0,1); // mode 5:[0101]     Fast PWM  8-bit - 0x00FF - BOTTOM - TOP
    T1_SET_COMA(1,0); // clear on match, set at BOTTOM (non-inverting)  

// TIMER0: run the interrupt at 200Hz
    OCR0A = 78; // COM value
    T0_SET_WGM(1,1,1); // [111] Fast PWM - OCR0A - BOTTOM - TOP
    T0_SET_COMB(1,0); // clear on match, set at BOTTOM (non-inverting)     |
    
    T1_SET_CS(0,0,1);
    T0_SET_CS(1,0,1); // prescaler Timer0 = 1024
}

// TIMSK0[TOIE0]    
//  - 15 0x001C TIMER0_COMPA_vect   Timer0 compare match A             -> TIMSK0[OCIE0A]
void init_interrupt()
{
    SET_INTERRUPT_ON(TIMSK0, OCIE0A); // Timer0 reach TOP (200Hz)
    ENABLE_INTERRUPT;
}

volatile int percent = 0;

// one full cycle (100 in 0.5s)
void TIMER0_COMPA_vect(void) __attribute__((signal, used));
void TIMER0_COMPA_vect(void)
{
    static int direction = 1;
    percent += direction;
    if (percent >= 100)
        direction = -1;
    else if (percent <= 0)
        direction = 1;
    OCR1A = 255 * percent / 100;
}

/*/////////////////////////////////////////////////////////////////////////////]
• You must configure Timer0 to trigger a periodic interrupt that varies the duty cycle
of the LED D2 controlled by Timer1.
• The frequency of Timer1 must be high enough to no longer see the LED blinking.
• Do not hesitate to use multiple Timer registers to complete this exercise.
• The duty cycle should vary in a loop from 0% to 100% and then from 100% to 0%
in 1 second      

LED D2 (PB1 / OC1A)
Output compare 1A > OCR1A
*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    SET_WRITING_PIN(DDRB, PB1);
    init_timer();
    init_interrupt();

    while (1) {}
}



