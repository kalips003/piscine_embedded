#include <avr/io.h>

ISR(TIMER1_COMPA_vect) {
    // print c
}

int main(void)
{


}


#define TOP (F_CPU / 1024) // 1s = 15625 Hz
// set ClockSelect CS (TCCR1B[2:0]) prescaler % 1024 => [101]; start the timer
#define PRESCALER_1024 (1 << CS12) | (0 << CS11) | (1 << CS10)


void set_WGM(char tccr1b_top, char tccr1a_lower) {

    TCCR1B |= tccr1b_top;
    TCCR1a |= tccr1a_lower;
}

void USART_Init( unsigned int ubrr)
{
/*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}


// Interupt Service Routime (ISR)
// isr-vector12: 0x0016 => timer1 compare match A; TIMER1_COMPA_vect;                                                                                             
// ISR = ;
void timer_init() {

    OCR1A = TOP;
    set_WGM();


    TCCR1B = PRESCALER_1024;


}


// 0x0016 => jmp instruction to ISR (?)
// ISR(TIMER1_COMPA_vect) makes the compiler name your function __vector_11.

The global interrupt flag is maintained in the I bit of the status register (SREG).
SREG(bit 7) = global interupt enable
sei();   // Set I bit   → interrupts on
cli();   // Clear I bit → interrupts off

// TIMSK1 is the Timer/Counter1 Interrupt Mask Register.
// bit:    7   6    5     4   3     2       1       0
//       [ – | – | ICIE1 | – | – | OCIE1B | OCIE1A | TOIE1 ]
/*
ICIE1	Input Capture	TIMER1_CAPT_vect	0x0014
OCIE1A	Compare Match A	TIMER1_COMPA_vect	0x0016
OCIE1B	Compare Match B	TIMER1_COMPB_vect	0x0018
*/
OCIE1A in 

void interupt_init() {

    TIMSK1 |= (1 << OCIE1A);
    sei();   // Set I bit   → interrupts on
}

For interrupt driven USART operation, the Global Interrupt Flag should be cleared (and interrupts
globally disabled) when doing the initialization.