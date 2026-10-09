#include <avr/io.h>

#define STOP_VALUE ((F_CPU / 256 / 2) - 1) // = 31249

int main(void)
{
// set PB1 output
    DDRB |= (1 << PB1); 

// set compare value (~0.5s)
    OCR1A = STOP_VALUE; 

// CompareMatchOUtput COM [01]: Toggle OC1A (PB1) on Compare Match
    TCCR1A = (0 << COM1A1) | (1 << COM1A0);

// set WaveformGenerationMode WGM [0100]:
    // mode: CTC; TOP: OCR1A; update: Immediate; TOV1 flag on:  MAX
    TCCR1B = (0 << WGM13) | (1 << WGM12);
    // TCCR1A = (0 << WGM11) | (0 << WGM10);

// set ClockSelect CS % 256: [100] & start the timer
    TCCR1B |= (1 << CS12) |  (0 << CS11) |  (0 << CS10);

    while (1) {}
}
