#include <avr/io.h>
#include <util/delay.h>

// Prescaler 256: 16 000 000 / 256 = 62 500 ticks per second
#define _01S (F_CPU / 256 / 10)
#define TOP_1S ((F_CPU / 256) - 1)
#define MATCH_100MS ((F_CPU / 256 / 10) - 1)

void init_timer() 
{
// set PB1 (OC1A) output
    DDRB |= (1 << PB1);
// set PD2 / PD4 (SW1 / SW2) as input
    DDRD &= ~((1 << PD2) | (1 << PD4));

// Values to compare TCNT1 against (set before starting the timer)
    ICR1  = TOP_1S;        // TOP: end of the 1 s period, TCNT1 back to 0
    OCR1A = MATCH_100MS;   // match point: LED turns off after 0.1 s

//  WGM1 (TCCR1A / TCCR1B): Waveform Generation Mode
    //   -> mode 14 [1110] = Fast PWM, TOP = ICR1, pin off on match with OCR1A
    TCCR1B = (1 << WGM13) | (1 << WGM12);
    TCCR1A = (1 << WGM11) | (0 << WGM10);


// CompareMatchOUtput COM [10]: Toggle OC1A (PB1) on Compare Match
// COM1A (TCCR1A): What pin OC1A (PB1) does on events (match with OCR1A, BOTTOM)
    //  COM1A[1/0] = 10 (bits 7-6): Fast PWM non-inverting:
        // Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
        // makes pin OC1A (PB1):
        // go HIGH at BOTTOM, when TCNT1 restarts from 0
        // go LOW on match, when TCNT1 == OCR1A
    TCCR1A |= (1 << COM1A1) | (0 << COM1A0);

// set ClockSelect CS % 256: [100] & start the timer
    TCCR1B |= (1 << CS12) |  (0 << CS11) |  (0 << CS10);
}

int main(void)
{
    init_timer();

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