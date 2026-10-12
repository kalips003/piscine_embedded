#pragma once

#include <avr/io.h>


/////////////////////////////////////////////////////////////////////////////]
// at 16MHz, we would need to count 15.999.999 into TCNT1, but its [0:65535]
// OCR1A = (F_CPU / prescaler / target_frequency) - 1
// ICR1 = (F_CPU / prescaler / target_frequency) - 1
#define STOP_1S   ((F_CPU / 256UL / 1UL) - 1) // 1s, 1 Hz, 62499 at 16 MHz
#define STOP_05S  ((F_CPU / 256UL / 2UL) - 1) // 0.5s, 2 Hz, 31249 at 16 MHz
#define STOP_025S ((F_CPU / 256UL / 4UL) - 1) // 0.25s

/////////////////////////////////////////////////////////////////////////////]
/*/ Set ClockSelect CS (TCCR1B[2:0]): set prescaler & start the timer
 - [000]: No clock (Timer stopped)
 - [001]: clk/1
 - [010]: clk/8
 - [011]: clk/64
 - [100]: clk/256
 - [101]: clk/1024
 - [110]: External clock, falling edge
 - [111]: External clock, rising edge                                       */ 
#define SET_CS(b2, b1, b0) \
    (TCCR1B = (TCCR1B & ~(_BV(CS12)|_BV(CS11)|_BV(CS10)))|((b2<<CS12)|(b1<<CS11)|(b0<<CS10)))

#define SET_CS_PRESCALER_1024 SET_CS(1,0,1)
#define SET_CS_PRESCALER_256 SET_CS(1,0,0)


/*/////////////////////////////////////////////////////////////////////////////]
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
*//////////////////////////////////////////////////////////////////////////////]

/////////////////////////////////////////////////////////////////////////////]
/*/ Set WaveformGenerationMode WGM across TCCR1B[4:3] & TCCR1A[1:0]
- [0000]: Normal
- [0001]: Phase Correct PWM, 8-bit
- [0010]: Phase Correct PWM, 9-bit
- [0011]: Phase Correct PWM, 10-bit
- [0100]: CTC (TOP = OCR1A)
- [0101]: Fast PWM, 8-bit
- [0110]: Fast PWM, 9-bit
- [0111]: Fast PWM, 10-bit
- [1000]: Phase and Frequency Correct PWM (TOP = ICR1)
- [1001]: Phase and Frequency Correct PWM (TOP = OCR1A)
- [1010]: Phase Correct PWM (TOP = ICR1)
- [1011]: Phase Correct PWM (TOP = OCR1A)
- [1100]: CTC (TOP = ICR1)
- [1101]: Reserved
- [1110]: Fast PWM (TOP = ICR1)
- [1111]: Fast PWM (TOP = OCR1A)                                            */
// CTC (Clear Timer on Compare Match): 
//      resets TCNT1 to 0 when it reaches OCR1A.
// PWM = Pulse-Width Modulation.
//      It's a technique for controlling the average power delivered to a device by switching a digital output ON and OFF very quickly
// Phase Correct PWM: 
//      counts up to TOP, then down to 0; produces symmetrical PWM.
//      (OCR1A/OCR1B) is updated at TOP.
// Fast PWM: 
//      counts up to TOP, then resets to 0; produces faster PWM.
// Phase and Frequency Correct PWM: 
//      counts up to TOP, then down to 0; keeps PWM phase and frequency stable.
//      (OCR1A/OCR1B) is updated at BOT
#define SET_WGM(b3, b2, b1, b0) do { \
            TCCR1B = (TCCR1B & ~(_BV(WGM13)|_BV(WGM12)))|((b3<<WGM13)|(b2<<WGM12)); \
            TCCR1A = (TCCR1A & ~(_BV(WGM11)|_BV(WGM10)))|((b1<<WGM11)|(b0<<WGM10)); \
        } while (0)


/////////////////////////////////////////////////////////////////////////////]
/*/ Set CompareMatchOutput COM on TCCR1A[7:6]
 - [00]: Normal port operation
 - [01]: Toggle OC1A on compare match (non-PWM modes); Toggle OC1A/OC1B in certain PWM modes
 - [10]: Clear OC1A/OC1B on compare match
 - [11]: Set OC1A/OC1B on compare match                                       */
// 0C1A = PB1; OC1B = PB2
#define SET_COM(b1, b0) (TCCR1A = TCCR1A & ~(_BV(COM1A1)|_BV(COM1A0))|(b1<<COM1A1)|(b0<<COM1A0))






