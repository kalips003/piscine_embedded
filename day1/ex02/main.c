#include <avr/io.h>

#define TOP_1S ((F_CPU / 256) - 1)
#define MATCH_100MS ((F_CPU / 256 / 10) - 1)

int main(void)
{
// set PB1 (OC1A) output
    DDRB |= (1 << PB1);

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

    while (1) {}
}


/*
A timer is just a counter that goes up by 1 on every clock tick. 
The mode (the WGM bits) decides how far it counts and what it does at the end.

Normal: counts from 0 to the max (65535 on Timer1), overflows, 
    and starts again at 0. You get a flag (TOV1) on each overflow. 
    It’s simple, but you can’t choose the period.

CTC (Clear Timer on Compare): counts from 0 to your value (OCR1A), then goes back to 0. 
    This lets you pick an exact frequency. It’s what you used for 1 Hz.
    OCR1A  /|  /|  /|
          / | / | / |
       0 /  |/  |/  |

PWM is the idea behind the 3 other modes. The pin switches between on and off very fast, 
    and the fraction of time it’s on (the “duty cycle”) is what matters. 
    At 25% on, an LED looks dim. At 90% on, it looks bright. The timer flips the pin by itself, 
    and OCR1x sets where it flips.

Fast PWM: counts up only, then jumps back to 0 (the sawtooth again). 
    The pin turns on at 0 and off when the counter passes OCR1x. 
    It’s the fastest PWM, and it’s fine for LEDs.
        counter  /|  /|          pin  ‾‾|___‾‾|___
        OCR ----/-|-/-|-

Phase correct PWM: counts up, then back down (a triangle). The pin flips when the counter 
    passes OCR on the way up and again on the way down. 
    The pulse sits centered in each cycle, so it’s more symmetric, which is better for motors. 
    The catch is half the frequency of Fast PWM
        counter  /\  /\          pin  _‾‾_ _‾‾_
        OCR ----/--\/--\-

Phase and frequency correct PWM: the same triangle as phase correct. 
    The only difference is when a new OCR or TOP value takes effect: at the bottom of the 
    triangle instead of the top. That only matters if you change the frequency while it runs: 
        this mode keeps every pulse symmetric, while phase correct can give one odd pulse. 
    If you never change the frequency, the two behave the same. Only Timer1 has this mode
*/