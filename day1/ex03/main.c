#include <avr/io.h>
#include <util/delay.h>

/*
the shape of the waveform:
So the shape depends on:
    the mode (WGM[3:0])
    then the pin action (COM[1:0]), 
    and the values (TOP and OCR1A).

*/

// Prescaler 256: 16 000 000 / 256 = 62 500 ticks per second
#define _01S          6250
#define TOP_1S        62499   // 62 500 ticks - 1 = 1 s period    -> ICR1
#define MATCH_100MS    6249   //  6 250 ticks - 1 = 0.1 s (10%)   -> OCR1A

int main(void)
{
// PB1 (OC1A) as output: DDRB bit 1
    DDRB |= (1 << PB1);
// PD2 / PD4 (SW1 / SW2) as input
    DDRD &= ~((1 << PD2) | (1 << PD4));    // PD2 = input  (button SW1 - SW2)

// Values to compare TCNT1 against (set before starting the timer)
    ICR1  = TOP_1S;        // TOP: end of the 1 s period, TCNT1 back to 0
    OCR1A = MATCH_100MS;   // match point: LED turns off after 0.1 s

//  WGM1 (TCCR1A / TCCR1B): Waveform Generation Mode
    //   -> mode 14 [1110] = Fast PWM, TOP = ICR1
    //  WGM1[3/2] = 11 (bits 4-3): high half of mode 14
    TCCR1B = (1 << WGM13) | (1 << WGM12);
    //  WGM1[1/0] = 10 (bits 1-0): low half of mode 14
    TCCR1A = (1 << WGM11) | (0 << WGM10);

// COM1A (TCCR1A):
    //  COM1A: What pin OC1A (PB1) does on events (match with OCR1A, BOTTOM)
    //  COM1A[1/0] = 10 (bits 7-6): Fast PWM non-inverting:
        // makes pin OC1A (PB1):
        // go HIGH at BOTTOM, when TCNT1 restarts from 0
        // go LOW on match, when TCNT1 == OCR1A
    TCCR1A |= (1 << COM1A1) | (0 << COM1A0);
    //  WGM1: The low half of the mode

// CS (TCCR1B):
    //  CS1[2/1/0] = 100 (bits 2-0): prescaler 256
    //      -> starts the timer, so this is done last
    TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);


    int cycle_speed = 1;

    int button1_idle = PIND & (1 << PD2);
    int button2_idle = PIND & (1 << PD4);

    while (1) {
    
        int button1_pressed = !(PIND & (1 << PD2));
        int button2_pressed = !(PIND & (1 << PD4));

        if (button1_idle && button1_pressed && cycle_speed < 10) { // increment

            cycle_speed++;
            OCR1A = _01S * cycle_speed - 1;
        }
        if (button2_idle && button2_pressed && cycle_speed > 1) { // decrement

            cycle_speed--;
            OCR1A = _01S * cycle_speed - 1;
        }

        button1_idle = PIND & (1 << PD2);
        button2_idle = PIND & (1 << PD4);
        _delay_ms(50);
    }
}