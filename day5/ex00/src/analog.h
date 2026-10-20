#pragma once

#include "lib.h"
#include <avr/io.h>

/* view ADC.png */

/*/////////////////////////////////////////////////////////////////////////////]
    # ADC Control and Status Register A

    ADEN (bit 7): Enables the ADC.
    ADSC (bit 6): Starts an ADC conversion.
    ADATE (bit 5): Enables automatic conversion triggering.
    ADIF (bit 4): Flags that a conversion has completed; clear by writing 1.
    ADIE (bit 3): Enables ADC conversion-complete interrupts.
    ADPS[2:0] (bits 2–0): Selects the ADC clock prescaler.
----------------------------------------------------------------------------
[ADCSRA] : ADEN - ADSC - ADATE - ADIF - ADIE - ADPS2 - ADPS1 - ADPS0
----------------------------------------------------------------------------
	Turn it on, start a conversion, know when it is done, set its clock.

* ADEN      : ADC Enable. [0] = off (and a conversion in progress is killed)
* ADSC      : ADC Start Conversion. Write 1 to start, it reads 1 while converting,
              hardware puts it back to 0 when done -> while (ADCSRA & _BV(ADSC));
* ADATE     : Auto Trigger Enable: start conversions on the event chosen in ADCSRB[ADTS]
* ADIF      : ADC Interrupt Flag, set when a conversion is done.
              Cleared by the ISR, or by writing 1 to it (yes, 1)
* ADIE      : ADC Interrupt Enable -> ADC_vect
* ADPS[2:0] : Prescaler, ADC clock = F_CPU / div
               [000] = /2    [001] = /2    [010] = /4    [011] = /8
               [100] = /16   [101] = /32   [110] = /64   [111] = /128
              For full 10-bit precision the ADC clock must be 50kHz..200kHz
              (faster is ok if you only want 8 bits, up to ~1MHz)
*//////////////////////////////////////////////////////////////////////////////]
#define ADC_ENABLE (SET_PIN(ADCSRA, ADEN))
// 
#define ADC_START (SET_PIN(ADCSRA, ADSC))
#define ADC_WAIT_RESULT do {} while (IS_BIT_ON(ADCSRA, ADSC))
// 
// The datasheet recommends an ADC clock of 50–200 kHz for maximum 10-bit resolution
// 16MHz / 128 = 125kHz -> the only valid one at 16MHz
#define ADC_SET_PRESCALER(b2,b1,b0) (ADCSRA = (ADCSRA & ~0b11000000) | ((b2<<ADPS2)|(b1<<ADPS1)|(b0<<ADPS0)))


/*/////////////////////////////////////////////////////////////////////////////]
    # ADC Multiplexer Selection Register

    7:6	REFS[1:0]	Select the ADC reference voltage
    5	ADLAR	    Adjust how the conversion result is stored
    3:0	MUX[3:0]	Select the input channel or internal source
----------------------------------------------------------------------------
[ADMUX] : REFS1 - REFS0 - ADLAR - – - MUX3 - MUX2 - MUX1 - MUX0
----------------------------------------------------------------------------
	Which voltage is "full scale", how the result is aligned, and which input.

* REFS[1:0] : Reference Selection (what 1023 means)
               [00] = AREF pin, internal ref off (external voltage on AREF)
               [01] = AVCC (5V on the board), with a capacitor on AREF
               [10] = reserved
               [11] = internal 1.1V, with a capacitor on AREF
* ADLAR     : ADC Left Adjust Result
               [0] = right adjusted: ADCH = ------98  ADCL = 76543210
               [1] = left adjusted:  ADCH = 98765432  ADCL = 10------
                     -> 8-bit mode = ADLAR=1 and read ADCH only, drop the 2 LSB
* MUX[3:0]  : Analog Channel Selection
               [0000:0101] = ADC0..ADC5 (PC0..PC5)
               [0110:0111] = ADC6, ADC7 (TQFP/QFN package only, no digital pin)
               [1000]      = ADC8, internal temperature sensor
               [1110]      = 1.1V (VBG)
               [1111]      = 0V (GND)
               others      = reserved
*//////////////////////////////////////////////////////////////////////////////]
#define ADC_SET_REF_SELECTION(b1, b0) (ADMUX = (ADMUX & ~0b11000000) | ((b1<<REFS1) | (b0<<REFS0)))
// 
#define SET_READING_8BITS (SET_PIN(ADMUX, ADLAR))
#define SET_READING_10BITS (UNSET_PIN(ADMUX, ADLAR))
// 
#define ADC_SET_ANALOG_CHANNEL_SELECTION(b3,b2,b1,b0) (ADMUX = (ADMUX & ~0b1111)|((b3<<MUX3)|(b2<<MUX2)|(b1<<MUX1)|(b0<<MUX0)))

/*/////////////////////////////////////////////////////////////////////////////]
    # ADC Data Register
----------------------------------------------------------------------------
[ADC] = [ADCH]:[ADCL]       result (16-bit access, the compiler reads L then H)
----------------------------------------------------------------------------
	If you read the 2 bytes by hand: ADCL FIRST, then ADCH.
	Reading ADCL locks the register until ADCH is read (so the 2 halves come
	from the same conversion). Read ADCL and forget ADCH = the ADC never
	updates the result again.
	In 8-bit mode (ADLAR=1) read only ADCH, that is allowed.
*//////////////////////////////////////////////////////////////////////////////]
