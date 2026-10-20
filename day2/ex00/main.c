#include "src/uart.h"
#include "src/timer.h"
#include "src/interrupt.h"

/////////////////////////////////////////////////////////////////////////////]
// == ISR(TIMER1_COMPA_vect) { uart_tx('Z'); }
// set __vector_11 (function called on timer1 compare match A)
void TIMER1_COMPA_vect(void) __attribute__((signal));
void TIMER1_COMPA_vect(void)
{
    uart_tx('Z');
}

/////////////////////////////////////////////////////////////////////////////]
// Reset and interupt vector
// TIMSK1 is the Timer/Counter1 Interrupt Mask Register.
    // OCIE1A: 0x0016: TIMER1 COMPA Timer/Counter1 Compare Match A
void interupt_init( void ) 
{
    TIMER1_COMPA_INT_ON; // enable interrupt for timer compare match A
    sei(); // Set I bit   → interrupts on
}


/////////////////////////////////////////////////////////////////////////////]
// 1 second loop, set OC1A on top (later let interupt on OC1A set)
void timer_init_1s( void ) {

    OCR1A = STOP_1S;
// set WGM mode 4: CTC, TOP=OCR1A, update: immediate, TOP
    T1_SET_WGM(0,1,0,0);
// prescaler 1024, start the timer
    T1_SET_CS(1,0,0);
}

/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    timer_init_1s();
    interupt_init();

    while (1) {}
}
