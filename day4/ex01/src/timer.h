#pragma once

#include <avr/io.h>



/*/////////////////////////////////////////////////////////////////////////////]
                                    TIMER 1
*//////////////////////////////////////////////////////////////////////////////]


/*/////////////////////////////////////////////////////////////////////////////]
    # 16-bit registers: Timer/Counter1, Output Compare 1 A/B, Input Capture 1
----------------------------------------------------------------
[TCNT1] = [TCNT1H]:[TCNT1L]      counter [0:65535]
[OCR1A] = [OCR1AH]:[OCR1AL]      compare A (or TOP)
[OCR1B] = [OCR1BH]:[OCR1BL]      compare B
[ICR1]  = [ICR1H]:[ICR1L]        input capture value (or TOP)
----------------------------------------------------------------
*//////////////////////////////////////////////////////////////////////////////]

// at 16MHz, we would need to count 15.999.999 into TCNT1, but its [0:65535]
// OCR1A = (F_CPU / prescaler / target_frequency) - 1
// ICR1 = (F_CPU / prescaler / target_frequency) - 1
#define STOP_1S   ((F_CPU / 256UL / 1UL) - 1) // 1s, 1 Hz, 62499 at 16 MHz
#define STOP_05S  ((F_CPU / 256UL / 2UL) - 1) // 0.5s, 2 Hz, 31249 at 16 MHz
#define STOP_025S ((F_CPU / 256UL / 4UL) - 1) // 0.25s



/*/////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter1 Control Register A
----------------------------------------------------------------------------
[TCCR1A] : COM1A1 - COM1A0 - COM1B1 - COM1B0 - – - – - WGM11 - WGM10
----------------------------------------------------------------------------
	What the OC1A/OC1B pins do on a match, plus the low 2 bits of the mode.

* COM1A[1:0] : Compare Output Mode for OC1A (PB1)
* COM1B[1:0] : Compare Output Mode for OC1B (PB2)
                    | non-PWM (Normal/CTC) | Fast PWM              | Phase (& Freq) Correct
               [00] | pin is a normal GPIO | normal GPIO           | normal GPIO
               [01] | toggle on match      | A: toggle if mode     | A: toggle if mode
                    |                      |    14/15, else GPIO   |    9/11, else GPIO
                    |                      | B: GPIO               | B: GPIO
               [10] | clear on match       | clear on match,       | clear on match going up,
                    |                      | set at BOTTOM         | set on match going down
                    |                      | (non-inverting)       | (non-inverting)
               [11] | set on match         | inverting             | inverting
*//////////////////////////////////////////////////////////////////////////////]

// Set CompareOutputMode COM1A (TCCR1A[7:6]) / COM1B (TCCR1A[5:4])
#define T1_SET_COMA(b1, b0) \
    (TCCR1A = (TCCR1A & ~(_BV(COM1A1)|_BV(COM1A0)))|((b1<<COM1A1)|(b0<<COM1A0)))
#define T1_SET_COMB(b1, b0) \
    (TCCR1A = (TCCR1A & ~(_BV(COM1B1)|_BV(COM1B0)))|((b1<<COM1B1)|(b0<<COM1B0)))


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter1 Control Register B
----------------------------------------------------------------------------
[TCCR1B] : ICNC1 - ICES1 - – - WGM13 - WGM12 - CS12 - CS11 - CS10
----------------------------------------------------------------------------
	Input capture setup, high 2 bits of the mode, and the clock select
	(writing CS != 0 starts the timer).

* ICNC1    : Input Capture Noise Canceler, edge must be stable 4 ticks (adds 4 ticks delay)
* ICES1    : Input Capture Edge Select, [0] = falling, [1] = rising
* WGM1[3:2]: Waveform Generation Mode bits 3:2 + TCCR1A(WGM1)[1:0]
               WGM1[3:0]  mode                      TOP      OCR1x update  TOV1 set at
          0    [0000]     Normal                    0xFFFF   immediate     MAX
          1    [0001]     Phase Correct  8-bit      0x00FF   TOP           BOTTOM
          2    [0010]     Phase Correct  9-bit      0x01FF   TOP           BOTTOM
          3    [0011]     Phase Correct 10-bit      0x03FF   TOP           BOTTOM
          4    [0100]     CTC                       OCR1A    immediate     MAX
          5    [0101]     Fast PWM  8-bit           0x00FF   BOTTOM        TOP
          6    [0110]     Fast PWM  9-bit           0x01FF   BOTTOM        TOP
          7    [0111]     Fast PWM 10-bit           0x03FF   BOTTOM        TOP
          8    [1000]     Phase & Freq Correct      ICR1     BOTTOM        BOTTOM
          9    [1001]     Phase & Freq Correct      OCR1A    BOTTOM        BOTTOM
          10   [1010]     Phase Correct             ICR1     TOP           BOTTOM
          11   [1011]     Phase Correct             OCR1A    TOP           BOTTOM
          12   [1100]     CTC                       ICR1     immediate     MAX
          13   [1101]     reserved
          14   [1110]     Fast PWM                  ICR1     BOTTOM        TOP
          15   [1111]     Fast PWM                  OCR1A    BOTTOM        TOP
           TOP = OCR1A: OCR1A is used up, only OC1B can output PWM
           TOP = ICR1 : both OC1A and OC1B free, but input capture is disabled
* CS1[2:0] : Clock Select, prescaler (same table as Timer0)
               [000] = stopped   [001] = clk/1    [010] = clk/8     [011] = clk/64
               [100] = clk/256   [101] = clk/1024
               [110] = T1 pin (PD5) falling edge  [111] = T1 pin rising edge
*//////////////////////////////////////////////////////////////////////////////]

// Set WaveformGenerationMode WGM1 across TCCR1B[4:3] & TCCR1A[1:0]
#define T1_SET_WGM(b3, b2, b1, b0) do { \
            TCCR1B = (TCCR1B & ~(_BV(WGM13)|_BV(WGM12)))|((b3<<WGM13)|(b2<<WGM12)); \
            TCCR1A = (TCCR1A & ~(_BV(WGM11)|_BV(WGM10)))|((b1<<WGM11)|(b0<<WGM10)); \
        } while (0)

// Set ClockSelect CS1 (TCCR1B[2:0]): set prescaler & start the timer
#define T1_SET_CS(b2, b1, b0) \
    (TCCR1B = (TCCR1B & ~(_BV(CS12)|_BV(CS11)|_BV(CS10)))|((b2<<CS12)|(b1<<CS11)|(b0<<CS10)))


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter1 Control Register C
----------------------------------------------------------------------------
[TCCR1C] : FOC1A - FOC1B - – - – - – - – - – - –
----------------------------------------------------------------------------
* FOC1A/B  : Force Output Compare, fake a match on OC1x now (non-PWM only, no interrupt)
*//////////////////////////////////////////////////////////////////////////////]


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter1 Interrupt Mask / Interrupt Flag Registers
----------------------------------------------------------------------------
[TIMSK1] : – - – - ICIE1 - – - – - OCIE1B - OCIE1A - TOIE1
[TIFR1]  : – - – - ICF1  - – - – - OCF1B  - OCF1A  - TOV1
----------------------------------------------------------------------------
	TIFR1 flags are set by hardware; if the matching TIMSK1 bit is on (and sei()),
	the ISR runs and the flag clears itself. Otherwise poll it and clear it
	by writing 1 to it (yes, 1).

* ICIE1  / ICF1  : Input Capture    -> TIMER1_CAPT_vect    (edge on ICP1, TCNT1 copied to ICR1)
                                                           (in ICR1-TOP modes: set at TOP instead)
* OCIE1B / OCF1B : Compare Match B  -> TIMER1_COMPB_vect
* OCIE1A / OCF1A : Compare Match A  -> TIMER1_COMPA_vect   (once per period in CTC)
* TOIE1  / TOV1  : Overflow         -> TIMER1_OVF_vect
*//////////////////////////////////////////////////////////////////////////////]




/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
                                    TIMER 0/2
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////]

/*/////////////////////////////////////////////////////////////////////////////]
    # Output Compare Register 0 A / B
----------------------------------------------------------------
[OCR0A] [OCR0B] : 8-bit compare values
----------------------------------------------------------------
	Compared to TCNT0 continuously. A match sets OCF0x, and can flip OC0x.
	OCR0A can also be TOP (CTC, or PWM modes with WGM02 = 1): then OCR0B
	is the only one left for a PWM output.

	! PWM modes: double buffered, the new value is taken at TOP or BOTTOM (see WGM table)
	! Normal/CTC: taken immediately. Writing below TCNT0 misses the match:
	  the counter goes all the way to 0xFF before coming back.
*//////////////////////////////////////////////////////////////////////////////]


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter0 Control Register A
----------------------------------------------------------------------------
[TCCR0A] : COM0A1 - COM0A0 - COM0B1 - COM0B0 - – - – - WGM01 - WGM00
----------------------------------------------------------------------------
	What the OC0A/OC0B pins do on a match, plus the low 2 bits of the mode.

* COM0A[1:0] : Compare Output Mode for OC0A (PD6)
* COM0B[1:0] : Compare Output Mode for OC0B (PD5)
                    | non-PWM (Normal/CTC) | Fast PWM            | Phase Correct PWM
               [00] | pin is a normal GPIO | normal GPIO         | normal GPIO
               [01] | toggle on match      | A: toggle if WGM02  | A: toggle if WGM02
                    |                      | B: reserved         | B: reserved
               [10] | clear on match       | clear on match,     | clear on match going up,
                    |                      | set at BOTTOM       | set on match going down
                    |                      | (non-inverting)     | (non-inverting)
               [11] | set on match         | inverting           | inverting
* WGM0[1:0]  : Waveform Generation Mode, with WGM02 in TCCR0B (see below)
*//////////////////////////////////////////////////////////////////////////////]

// Set CompareOutputMode COM0A (TCCR0A[7:6]) / COM0B (TCCR0A[5:4])
#define T0_SET_COMA(b1, b0) \
    (TCCR0A = (TCCR0A & ~(_BV(COM0A1)|_BV(COM0A0)))|((b1<<COM0A1)|(b0<<COM0A0)))
#define T0_SET_COMB(b1, b0) \
    (TCCR0A = (TCCR0A & ~(_BV(COM0B1)|_BV(COM0B0)))|((b1<<COM0B1)|(b0<<COM0B0)))
#define T2_SET_COMA(b1, b0) \
    (TCCR2A = (TCCR2A & ~(_BV(COM2A1)|_BV(COM2A0)))|((b1<<COM2A1)|(b0<<COM2A0)))
#define T2_SET_COMB(b1, b0) \
    (TCCR2A = (TCCR2A & ~(_BV(COM2B1)|_BV(COM2B0)))|((b1<<COM2B1)|(b0<<COM2B0)))



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter0 Control Register B
----------------------------------------------------------------------------
[TCCR0B] : FOC0A - FOC0B - – - – - WGM02 - CS02 - CS01 - CS00
----------------------------------------------------------------------------
	High bit of the mode, and the clock select (writing CS != 0 starts the timer).

* FOC0A/B  : Force Output Compare, fake a match on OC0x now (non-PWM only, no interrupt)
* WGM02    : Waveform Generation Mode bit 2
               WGM0[2:0]  mode                 TOP     OCR0x update  TOV0 set at
               [000]      Normal               0xFF    immediate     MAX
               [001]      Phase Correct PWM    0xFF    TOP           BOTTOM
               [010]      CTC                  OCR0A   immediate     MAX
               [011]      Fast PWM             0xFF    BOTTOM        MAX
               [101]      Phase Correct PWM    OCR0A   TOP           BOTTOM
               [111]      Fast PWM             OCR0A   BOTTOM        TOP
               [100] [110] reserved
* CS0[2:0] : Clock Select, prescaler. Same table as Timer1, NOT Timer2
               [000] = stopped   [001] = clk/1    [010] = clk/8     [011] = clk/64
               [100] = clk/256   [101] = clk/1024
               [110] = T0 pin (PD4) falling edge  [111] = T0 pin rising edge
*//////////////////////////////////////////////////////////////////////////////]

// Set WaveformGeneratidesktop_backgroundonMode WGM0 across TCCR0B[3] & TCCR0A[1:0]
#define T0_SET_WGM(b2, b1, b0) do { \
            TCCR0B = (TCCR0B & ~_BV(WGM02))|(b2<<WGM02); \
            TCCR0A = (TCCR0A & ~(_BV(WGM01)|_BV(WGM00)))|((b1<<WGM01)|(b0<<WGM00)); \
        } while (0)
#define T2_SET_WGM(b2, b1, b0) do { \
            TCCR2B = (TCCR2B & ~_BV(WGM22))|(b2<<WGM22); \
            TCCR2A = (TCCR2A & ~(_BV(WGM21)|_BV(WGM20)))|((b1<<WGM21)|(b0<<WGM20)); \
        } while (0)

// Set ClockSelect CS1 (TCCR1B[2:0]): set prescaler & start the timer
#define T0_SET_CS(b2, b1, b0) \
    (TCCR0B = (TCCR0B & ~(_BV(CS02)|_BV(CS01)|_BV(CS00)))|((b2<<CS02)|(b1<<CS01)|(b0<<CS00)))
#define T2_SET_CS(b2, b1, b0) \
    (TCCR2B = (TCCR2B & ~(_BV(CS22)|_BV(CS21)|_BV(CS20)))|((b2<<CS22)|(b1<<CS21)|(b0<<CS20)))


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # Timer/Counter0 Interrupt Mask / Interrupt Flag Registers
----------------------------------------------------------------------------
[TIMSK0] : – - – - – - – - – - OCIE0B - OCIE0A - TOIE0
[TIFR0]  : – - – - – - – - – - OCF0B  - OCF0A  - TOV0
----------------------------------------------------------------------------
	TIFR0 flags are set by hardware; if the matching TIMSK0 bit is on (and sei()),
	the ISR runs and the flag clears itself. Otherwise poll it and clear it
	by writing 1 to it (yes, 1).

* OCIE0B / OCF0B : Compare Match B  -> TIMER0_COMPB_vect
* OCIE0A / OCF0A : Compare Match A  -> TIMER0_COMPA_vect   (once per period in CTC)
* TOIE0  / TOV0  : Overflow         -> TIMER0_OVF_vect
*//////////////////////////////////////////////////////////////////////////////]





/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
*//////////////////////////////////////////////////////////////////////////////]

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





