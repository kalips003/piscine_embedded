#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>  // sei(), cli(), ISR(), every XXX_vect name
#include <util/atomic.h>    // ATOMIC_BLOCK()


/*/////////////////////////////////////////////////////////////////////////////]
An interrupt is the hardware tapping the CPU on the shoulder:
    "something happened, stop what you're doing and handle it".
Instead of looping and checking a flag yourself (polling), you write a function (an ISR),
    and the CPU jumps into it by itself when the event happens.

Every interrupt source has 2 bits:
    - a FLAG   (in a xxxFR / status register): set by the hardware when the event happens,
                    whether you care or not.
    - an ENABLE (in a xxxMSK / control register): you set it to say "call my ISR on this flag".
And above all of them there is one master switch: the I bit in SREG.
 - sei(): Set Interrupt flag   -> interrupts are allowed
 - cli(): Clear Interrupt flag -> every interrupt is blocked (flags still get set,
                                  they will fire as soon as sei() is called again)

An ISR runs only if: FLAG == 1 && ENABLE == 1 && SREG[I] == 1

What happens then:
    1. the CPU finishes the current instruction
    2. it pushes the return address (PC) on the stack
    3. it clears SREG[I] (so another interrupt can't cut in the middle of your ISR)
    4. it jumps to the vector of that interrupt (a fixed address at the start of flash)
    5. the vector holds a jump to your ISR() function
    6. your ISR runs; at the end the "reti" instruction pops PC and sets SREG[I] back
    7. main() continues exactly where it was, it never knows it was interrupted
    It takes at least 4 clock cycles to get in, and 4 to get out (+ the register saving gcc adds)

Most flags are cleared automatically when the ISR is entered.
    Some are not (USART RXC0 is cleared only by reading UDR0, UDRE0 only by writing UDR0):
        if you don't do it, the ISR is called again, and again, forever.
To clear a flag by hand, you write a 1 into it (yes, a 1): EIFR = _BV(INTF0);

        main()  ───────┐               ┌──────────────>
                       │  flag + I=1   │ reti
                       └──> ISR() ─────┘
*//////////////////////////////////////////////////////////////////////////////]


/////////////////////////////////////////////////////////////////////////////]

#define NO_INTERRUP __signal__
#define USED __used__
#define VISIBLE __externally_visible__

// Write the ISR:
// void	PCINT2_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));
// void	PCINT2_vect(void) {}

// OR:     ISR(TIMER1_COMPA_vect) { ... }
// The name must match a vector from the table below EXACTLY: a typo only gives a warning
//      ("appears to be a misspelled signal handler") and your interrupt jumps to
//      __bad_interrupt, which resets the chip.


/*/////////////////////////////////////////////////////////////////////////////]
volatile: every variable shared between an ISR and main() MUST be volatile.
    volatile uint8_t g_count = 0;

Without it, gcc sees that main() never writes g_count inside the loop, keeps it in a register,
    and never reads RAM again: your ISR changes it, main() never sees it.
    volatile = "this can change behind your back, read it from memory every time".

Atomic: the AVR is 8-bit. A 16-bit variable is read in 2 instructions.
    If the ISR fires between the 2, you get the low byte of the old value
    and the high byte of the new one (0x00FF -> 0x0100 can be read as 0x01FF).
    Protect multi-byte reads/writes of shared variables:
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { copy = g_count16; }
    It does cli() before, and puts SREG back as it was after.

Rules for an ISR:
    - keep it short: set a flag / store a byte, do the real work in main()
    - no _delay_ms(), no long loops, no waiting for the UART inside
    - while it runs, every other interrupt waits
*//////////////////////////////////////////////////////////////////////////////]


/////////////////////////////////////////////////////////////////////////////]
/*/ Interrupt Vector Table (ATmega328P), lowest number = highest priority
  (if 2 flags are set at the same time, the lower one goes first)
 -  1 0x0000 RESET
 -  2 0x0002 INT0_vect           External pin INT0 (PD2)
 -  3 0x0004 INT1_vect           External pin INT1 (PD3)
 -  4 0x0006 PCINT0_vect         Pin Change group 0 (PORTB)
 -  5 0x0008 PCINT1_vect         Pin Change group 1 (PORTC)
 -  6 0x000A PCINT2_vect         Pin Change group 2 (PORTD)
 -  7 0x000C WDT_vect            Watchdog timeout
 -  8 0x000E TIMER2_COMPA_vect   Timer2 compare match A
 -  9 0x0010 TIMER2_COMPB_vect   Timer2 compare match B
 - 10 0x0012 TIMER2_OVF_vect     Timer2 overflow
 - 11 0x0014 TIMER1_CAPT_vect    Timer1 input capture
 - 12 0x0016 TIMER1_COMPA_vect   Timer1 compare match A
 - 13 0x0018 TIMER1_COMPB_vect   Timer1 compare match B
 - 14 0x001A TIMER1_OVF_vect     Timer1 overflow
 - 15 0x001C TIMER0_COMPA_vect   Timer0 compare match A
 - 16 0x001E TIMER0_COMPB_vect   Timer0 compare match B
 - 17 0x0020 TIMER0_OVF_vect     Timer0 overflow
 - 18 0x0022 SPI_STC_vect        SPI transfer complete
 - 19 0x0024 USART_RX_vect       USART byte received
 - 20 0x0026 USART_UDRE_vect     USART data register empty
 - 21 0x0028 USART_TX_vect       USART transmit complete
 - 22 0x002A ADC_vect            ADC conversion complete
 - 23 0x002C EE_READY_vect       EEPROM ready
 - 24 0x002E ANALOG_COMP_vect    Analog comparator
 - 25 0x0030 TWI_vect            I2C (TWI)
 - 26 0x0032 SPM_READY_vect      Store program memory ready                   */
// addresses are in words (2 bytes): each vector is one 2-word "jmp your_isr" instruction


/////////////////////////////////////////////////////////////////////////////]
/*/ Timer1 Interrupt Mask: TIMSK1
// bit:    7   6    5     4   3     2       1       0
//       [ – | – | ICIE1 | – | – | OCIE1B | OCIE1A | TOIE1 ]
 - ICIE1:  Input Capture       -> TIMER1_CAPT_vect   (flag ICF1  in TIFR1)
 - OCIE1B: Compare Match B     -> TIMER1_COMPB_vect  (flag OCF1B in TIFR1)
 - OCIE1A: Compare Match A     -> TIMER1_COMPA_vect  (flag OCF1A in TIFR1)
 - TOIE1:  Overflow            -> TIMER1_OVF_vect    (flag TOV1  in TIFR1)        */
// In CTC mode (TOP = OCR1A), OCIE1A fires once per period: with STOP_1S, once per second.
// Timer0 / Timer2 are the same with TIMSK0/TIFR0 and TIMSK2/TIFR2 (no input capture).
#define TIMER1_COMPA_INT_ON  (TIMSK1 |= _BV(OCIE1A))
#define TIMER1_COMPA_INT_OFF (TIMSK1 &= ~_BV(OCIE1A))
#define TIMER1_COMPB_INT_ON  (TIMSK1 |= _BV(OCIE1B))
#define TIMER1_COMPB_INT_OFF (TIMSK1 &= ~_BV(OCIE1B))
#define TIMER1_OVF_INT_ON    (TIMSK1 |= _BV(TOIE1))
#define TIMER1_OVF_INT_OFF   (TIMSK1 &= ~_BV(TOIE1))


/////////////////////////////////////////////////////////////////////////////]
/*/ USART Interrupt Enable: UCSR0B[7:5]
 - RXCIE0: Receive Complete  -> USART_RX_vect
            fires when a byte arrived (RXC0 = 1).
            You MUST read UDR0 in the ISR, it's the only thing that clears RXC0.
 - TXCIE0: Transmit Complete -> USART_TX_vect
            fires when the last frame is fully out (stop bit included) and UDR0 is empty.
            TXC0 is cleared automatically. Useful to know when you can turn the line off.
 - UDRIE0: Data Register Empty -> USART_UDRE_vect
            fires as long as UDR0 can take a new byte (UDRE0 = 1), which is... almost always.
            Turn it on when you have something to send, write the next byte in the ISR,
            and turn it OFF when your buffer is empty, or the ISR loops forever.        */
// careful: UCSR0B also holds RXEN0/TXEN0, use |= and &=, don't overwrite it with =
#define UART_RX_INT_ON    (UCSR0B |= _BV(RXCIE0))
#define UART_RX_INT_OFF   (UCSR0B &= ~_BV(RXCIE0))
#define UART_TX_INT_ON    (UCSR0B |= _BV(TXCIE0))
#define UART_TX_INT_OFF   (UCSR0B &= ~_BV(TXCIE0))
#define UART_UDRE_INT_ON  (UCSR0B |= _BV(UDRIE0))
#define UART_UDRE_INT_OFF (UCSR0B &= ~_BV(UDRIE0))


/////////////////////////////////////////////////////////////////////////////]
/*/ External Interrupt Control: EICRA[3:0], InterruptSenseControl
    ISC01:ISC00 for INT0 (PD2), ISC11:ISC10 for INT1 (PD3)
 - [00]: LOW level      (fires again and again while the pin stays low)
 - [01]: ANY change     (both edges)
 - [10]: FALLING edge   (1 -> 0, a button with pull-up being pressed)
 - [11]: RISING edge    (0 -> 1, a button with pull-up being released)          */
// Then enable it in EIMSK (INT1 = bit1, INT0 = bit0). Flags are in EIFR (INTF1, INTF0).
// Works even if the pin is set as output (writing the pin triggers it: software interrupt).
// Changing ISC can set the flag by itself: clear EIFR after configuring, before enabling.
#define SET_ISC0(b1, b0) \
    (EICRA = (EICRA & ~(_BV(ISC01)|_BV(ISC00)))|((b1<<ISC01)|(b0<<ISC00)))
#define SET_ISC1(b1, b0) \
    (EICRA = (EICRA & ~(_BV(ISC11)|_BV(ISC10)))|((b1<<ISC11)|(b0<<ISC10)))

#define SET_ISC0_FALLING SET_ISC0(1,0)
#define SET_ISC0_RISING  SET_ISC0(1,1)
#define SET_ISC0_CHANGE  SET_ISC0(0,1)

#define INT0_ON  (EIMSK |= _BV(INT0))
#define INT0_OFF (EIMSK &= ~_BV(INT0))
#define INT1_ON  (EIMSK |= _BV(INT1))
#define INT1_OFF (EIMSK &= ~_BV(INT1))
#define INT0_CLEAR_FLAG (EIFR = _BV(INTF0))  // write 1 to clear
#define INT1_CLEAR_FLAG (EIFR = _BV(INTF1))


/////////////////////////////////////////////////////////////////////////////]
/*/ Pin Change Interrupts: PCICR (enable a group) + PCMSKx (choose the pins)
 - PCIE0: PCINT[7:0]   = PB0..PB7 -> PCMSK0 -> PCINT0_vect
 - PCIE1: PCINT[14:8]  = PC0..PC6 -> PCMSK1 -> PCINT1_vect
 - PCIE2: PCINT[23:16] = PD0..PD7 -> PCMSK2 -> PCINT2_vect                     */
// Every pin can do it, but:
//      - always on ANY change (press AND release), no edge choice
//      - one vector for the whole port: the ISR has to read PINx to know which pin / which edge
// Flags in PCIFR (PCIF2, PCIF1, PCIF0).
// ex: PD4 is PCINT20 -> PCICR |= _BV(PCIE2); PCMSK2 |= _BV(PCINT20);
#define PCINT_PORTB_ON(pin) do { PCICR |= _BV(PCIE0); PCMSK0 |= _BV(pin); } while (0)
#define PCINT_PORTC_ON(pin) do { PCICR |= _BV(PCIE1); PCMSK1 |= _BV(pin); } while (0)
#define PCINT_PORTD_ON(pin) do { PCICR |= _BV(PCIE2); PCMSK2 |= _BV(pin); } while (0)
// pin = the bit number in its port (PD4 -> 4), same as PCINTxx & 7


