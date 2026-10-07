#include <avr/io.h>

#define STOP_VALUE 31249
/*
    TCCR1B = starting the timer + what % mode (3 least bit) + mode control
    OCR1A = stopping 
    COMIA10 != 0; timer takes over PB1 (toogle)
*/
int main(void)
{
    DDRB |= (1 << PB1);
    OCR1A = STOP_VALUE;
    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS12);

    while (1) {}
}


/*

On every match, the hardware always sets a flag: a bit that goes to 1 in 
    the TIFR1 register (OCF1A for a match with A, OCF1B for B). 
    That's just a note saying "a match happened".

It becomes a real interrupt, where the CPU stops what it's doing and 
    runs your function, only if you enable it. 
    The enable bit is OCIE1A in TIMSK1, and interrupts must also 
    be turned on globally with sei().

1	DDRB	bit PB1 = 1	Pin as output, or nothing comes out
2	OCR1A	31249	Compare value: end of the cycle in CTC
3	TCCR1A	COM1A0 = 1 (COM1A1 = 0)	Timer takes over PB1 and toggles it on each match
4	TCCR1B	WGM12 = 1 (bit 3)	CTC mode
4	TCCR1B	CS bits (0–2) = [100]	÷256

*/