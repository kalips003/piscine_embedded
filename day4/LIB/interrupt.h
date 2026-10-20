#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>  // sei(), cli(), ISR(), every XXX_vect name
#include <util/atomic.h>    // ATOMIC_BLOCK()


/*/////////////////////////////////////////////////////////////////////////////]
What happens in case of interrupt:
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
// Set I bit
#define ENABLE_INTERRUPT (SREG |= (1 << 7))
#define DISABLE_INTERRUPT (SREG &= ~(1 << 7))

/////////////////////////////////////////////////////////////////////////////]
/*/ __attribute__((...)):
 - signal             : ISR: save/restore used regs + SREG, return with reti 
                            (tell compiler this is an interrupt funciton)
 - used               : keep it even though no C code calls it
 - externally_visible : keep the global name __vector_N visible under LTO
*/

// Write the ISR:
// void	PCINT2_vect(void) __attribute__ ((signal, used, externally_visible));
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
 -  2 0x0002 INT0_vect           External pin INT0 (PD2)            -> EIMSK[INT0]
 -  3 0x0004 INT1_vect           External pin INT1 (PD3)            -> EIMSK[INT1]
 -  4 0x0006 PCINT0_vect         Pin Change group 0 (PORTB)         -> PCICR[PCIE0]
 -  5 0x0008 PCINT1_vect         Pin Change group 1 (PORTC)         -> PCICR[PCIE1]
 -  6 0x000A PCINT2_vect         Pin Change group 2 (PORTD)         -> PCICR[PCIE2]
 -  7 0x000C WDT_vect            Watchdog timeout                   -> WDTCSR[WDIE]
 -  8 0x000E TIMER2_COMPA_vect   Timer2 compare match A             -> TIMSK2[OCIE2A]
 -  9 0x0010 TIMER2_COMPB_vect   Timer2 compare match B             -> TIMSK2[OCIE2B]
 - 10 0x0012 TIMER2_OVF_vect     Timer2 overflow                    -> TIMSK2[TOIE2]
 - 11 0x0014 TIMER1_CAPT_vect    Timer1 input capture               -> TIMSK1[ICIE1]
 - 12 0x0016 TIMER1_COMPA_vect   Timer1 compare match A             -> TIMSK1[OCIE1A]
 - 13 0x0018 TIMER1_COMPB_vect   Timer1 compare match B             -> TIMSK1[OCIE1B]
 - 14 0x001A TIMER1_OVF_vect     Timer1 overflow                    -> TIMSK1[TOIE1]
 - 15 0x001C TIMER0_COMPA_vect   Timer0 compare match A             -> TIMSK0[OCIE0A]
 - 16 0x001E TIMER0_COMPB_vect   Timer0 compare match B             -> TIMSK0[OCIE0B]
 - 17 0x0020 TIMER0_OVF_vect     Timer0 overflow                    -> TIMSK0[TOIE0]
 - 18 0x0022 SPI_STC_vect        SPI transfer complete              -> SPCR[SPIE]
 - 19 0x0024 USART_RX_vect       USART byte received                -> UCSR0B[RXCIE0]
 - 20 0x0026 USART_UDRE_vect     USART data register empty          -> UCSR0B[UDRIE0]
 - 21 0x0028 USART_TX_vect       USART transmit complete            -> UCSR0B[TXCIE0]
 - 22 0x002A ADC_vect            ADC conversion complete            -> ADCSRA[ADIE]
 - 23 0x002C EE_READY_vect       EEPROM ready                       -> EECR[EERIE]
 - 24 0x002E ANALOG_COMP_vect    Analog comparator                  -> ACSR[ACIE]
 - 25 0x0030 TWI_vect            I2C (TWI)                          -> TWCR[ TWIE ]
 - 26 0x0032 SPM_READY_vect      Store program memory ready         -> SPMCSR[SPMIE]      */
// addresses are in words (2 bytes): each vector is one 2-word "jmp your_isr" instruction

#define SET_INTERRUPT_ON(register, bit)  (register |= _BV(bit))
#define SET_INTERRUPT_OFF(register, bit)  (register &= ~_BV(bit))




// Important distinctions:
// - PCINTx_vect: the group enable bit above is not enough by itself. You must also set the corresponding pin mask bit in PCMSK0, PCMSK1, or PCMSK2.
// - Timer interrupts: configure the timer mode and clock as needed; compare-match interrupts require the corresponding compare event to occur.
// - USART_RX_vect: reception must be enabled with UCSR0B[RXEN0] as well.
// - ADC_vect: the ADC and its interrupt must be enabled (ADEN and ADIE).
// - WDT_vect: watchdog interrupt mode must be configured, not merely the WDIE bit in isolation.


/////////////////////////////////////////////////////////////////////////////]
/* External Interrupt Mask: EIMSK
----------------------------------------------------------------------------
[EIMSK]      [ – – – – – – INT1 INT0 ]
----------------------------------------------------------------------------
- INT1: External Interrupt 1 -> INT1_vect (flag INTF1 in EIFR)
- INT0: External Interrupt 0 -> INT0_vect (flag INTF0 in EIFR) */

/* External Interrupt Control Register A: EICRA
[EICRA]      [ ISC11 ISC10 ISC01 ISC00 – – – – ]
- ISC1[1:0]: INT1 trigger condition
- ISC0[1:0]: INT0 trigger condition
- [00]: Low level
- [01]: Any logical change
- [10]: Falling edge
- [11]: Rising edge */

/* External Interrupt Flag Register: EIFR
[EIFR]       [ – – – – – – INTF1 INTF0 ]
- INTF1: Interrupt flag for INT1_vect
- INTF0: Interrupt flag for INT0_vect
- Flags are cleared by writing 1 to the corresponding bit */


/* Pin Change Interrupt Control Register: PCICR
----------------------------------------------------------------------------
[PCICR]      [ – – – – – PCIE2 PCIE1 PCIE0 ]
----------------------------------------------------------------------------
- PCIE2: Enable pin-change group 2 -> PCINT2_vect (PORTD)
- PCIE1: Enable pin-change group 1 -> PCINT1_vect (PORTC)
- PCIE0: Enable pin-change group 0 -> PCINT0_vect (PORTB) */


/* Pin Change Mask Register 0: PCMSK0
----------------------------------------------------------------------------
[PCMSK0]     [ PCINT7 PCINT6 PCINT5 PCINT4 PCINT3 PCINT2 PCINT1 PCINT0 ]
----------------------------------------------------------------------------
- Each bit enables the corresponding pin-change interrupt on PORTB
- Group interrupt enable: PCICR[PCIE0]
- Group flag: PCIFR[PCIF0] -> PCINT0_vect */


/* Pin Change Mask Register 1: PCMSK1
----------------------------------------------------------------------------
[PCMSK1]     [ – – PCINT14 PCINT13 PCINT12 PCINT11 PCINT10 PCINT9 PCINT8 ]
----------------------------------------------------------------------------
- Each bit enables the corresponding pin-change interrupt on PORTC
- Group interrupt enable: PCICR[PCIE1]
- Group flag: PCIFR[PCIF1] -> PCINT1_vect */


/* Pin Change Mask Register 2: PCMSK2
----------------------------------------------------------------------------
[PCMSK2]     [ PCINT23 PCINT22 PCINT21 PCINT20 PCINT19 PCINT18 PCINT17 PCINT16 ]
----------------------------------------------------------------------------
- Each bit enables the corresponding pin-change interrupt on PORTD
- Group interrupt enable: PCICR[PCIE2]
- Group flag: PCIFR[PCIF2] -> PCINT2_vect */


/* Pin Change Interrupt Flag Register: PCIFR
----------------------------------------------------------------------------
[PCIFR]      [ – – – – – PCIF2 PCIF1 PCIF0 ]
----------------------------------------------------------------------------
- PCIF2: Flag for PCINT2_vect
- PCIF1: Flag for PCINT1_vect
- PCIF0: Flag for PCINT0_vect
- Flags are cleared by writing 1 to the corresponding bit */


/* Watchdog Timer Control Register: WDTCSR
----------------------------------------------------------------------------
[WDTCSR]     [ WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0 ]
----------------------------------------------------------------------------
- WDIE: Watchdog interrupt enable -> WDT_vect
- WDIF: Watchdog interrupt flag
- WDE: Watchdog system-reset enable
- WDP3:WDP0: Watchdog timeout prescaler configuration */


/* Timer/Counter0 Interrupt Mask Register: TIMSK0
----------------------------------------------------------------------------
[TIMSK0]     [ – – – – – OCIE0B OCIE0A TOIE0 ]
----------------------------------------------------------------------------
- OCIE0B: Compare Match B interrupt enable -> TIMER0_COMPB_vect (OCF0B in TIFR0)
- OCIE0A: Compare Match A interrupt enable -> TIMER0_COMPA_vect (OCF0A in TIFR0)
- TOIE0: Overflow interrupt enable -> TIMER0_OVF_vect (TOV0 in TIFR0) */


/* Timer/Counter1 Interrupt Mask Register: TIMSK1
----------------------------------------------------------------------------
[TIMSK1]     [ – – ICIE1 – – OCIE1B OCIE1A TOIE1 ]
----------------------------------------------------------------------------
- ICIE1: Input Capture interrupt enable -> TIMER1_CAPT_vect (ICF1 in TIFR1)
- OCIE1B: Compare Match B interrupt enable -> TIMER1_COMPB_vect (OCF1B in TIFR1)
- OCIE1A: Compare Match A interrupt enable -> TIMER1_COMPA_vect (OCF1A in TIFR1)
- TOIE1: Overflow interrupt enable -> TIMER1_OVF_vect (TOV1 in TIFR1) */


/* Timer/Counter2 Interrupt Mask Register: TIMSK2
----------------------------------------------------------------------------
[TIMSK2]     [ – – – – – OCIE2B OCIE2A TOIE2 ]
----------------------------------------------------------------------------
- OCIE2B: Compare Match B interrupt enable -> TIMER2_COMPB_vect (OCF2B in TIFR2)
- OCIE2A: Compare Match A interrupt enable -> TIMER2_COMPA_vect (OCF2A in TIFR2)
- TOIE2: Overflow interrupt enable -> TIMER2_OVF_vect (TOV2 in TIFR2) */


/* Timer/Counter0 Interrupt Flag Register: TIFR0
----------------------------------------------------------------------------
[TIFR0]      [ – – – – – OCF0B OCF0A TOV0 ]
----------------------------------------------------------------------------
- OCF0B: Timer0 Compare Match B flag
- OCF0A: Timer0 Compare Match A flag
- TOV0: Timer0 Overflow flag
- Flags are cleared by writing 1 to the corresponding bit */


/* Timer/Counter1 Interrupt Flag Register: TIFR1
----------------------------------------------------------------------------
[TIFR1]      [ ICF1 – – – – OCF1B OCF1A TOV1 ]
----------------------------------------------------------------------------
- ICF1: Timer1 Input Capture flag
- OCF1B: Timer1 Compare Match B flag
- OCF1A: Timer1 Compare Match A flag
- TOV1: Timer1 Overflow flag
- Flags are cleared by writing 1 to the corresponding bit */


/* Timer/Counter2 Interrupt Flag Register: TIFR2
----------------------------------------------------------------------------
[TIFR2]      [ – – – – – OCF2B OCF2A TOV2 ]
----------------------------------------------------------------------------
- OCF2B: Timer2 Compare Match B flag
- OCF2A: Timer2 Compare Match A flag
- TOV2: Timer2 Overflow flag
- Flags are cleared by writing 1 to the corresponding bit */


/* SPI Control Register: SPCR
----------------------------------------------------------------------------
[SPCR]       [ SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0 ]
----------------------------------------------------------------------------
- SPIE: SPI interrupt enable -> SPI_STC_vect (SPIF in SPSR)
- SPE: SPI enable */


/* SPI Status Register: SPSR
----------------------------------------------------------------------------
[SPSR]       [ SPIF WCOL – – – – – SPI2X ]
----------------------------------------------------------------------------
- SPIF: SPI transfer complete flag -> SPI_STC_vect
- SPI2X: Double SPI speed */


/* USART Control and Status Register 0 B: UCSR0B
----------------------------------------------------------------------------
[UCSR0B]     [ RXCIE0 TXCIE0 UDRIE0 RXEN0 TXEN0 UCSZ02 RXB8 TXB8 ]
----------------------------------------------------------------------------
- RXCIE0: Receive complete interrupt enable -> USART_RX_vect (RXC0 in UCSR0A)
- TXCIE0: Transmit complete interrupt enable -> USART_TX_vect (TXC0 in UCSR0A)
- UDRIE0: Data register empty interrupt enable -> USART_UDRE_vect (UDRE0 in UCSR0A)
- RXEN0: Receiver enable
- TXEN0: Transmitter enable */


/* USART Control and Status Register 0 A: UCSR0A
----------------------------------------------------------------------------
[UCSR0A]     [ RXC0 TXC0 UDRE0 FE0 DOR0 UPE0 U2X0 MPCM0 ]
----------------------------------------------------------------------------
- RXC0: Receive complete flag
- TXC0: Transmit complete flag
- UDRE0: USART data register empty flag
- U2X0: Double-speed asynchronous mode */


/* ADC Control and Status Register A: ADCSRA
----------------------------------------------------------------------------
[ADCSRA]     [ ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0 ]
----------------------------------------------------------------------------
- ADIE: ADC interrupt enable -> ADC_vect (ADIF in ADCSRA)
- ADIF: ADC conversion complete flag
- ADEN: ADC enable
- ADSC: Start ADC conversion */


/* EEPROM Control Register: EECR
----------------------------------------------------------------------------
[EECR]       [ – – EEPM1 EEPM0 EERIE EEMPE EEPE EERE ]
----------------------------------------------------------------------------
- EERIE: EEPROM Ready interrupt enable -> EE_READY_vect
- EEPE: EEPROM programming enable
- EERE: EEPROM read enable */


/* Analog Comparator Control and Status Register: ACSR
----------------------------------------------------------------------------
[ACSR]       [ ACD ACBG ACO ACI ACIE ACIC ACIS1 ACIS0 ]
----------------------------------------------------------------------------
- ACIE: Analog Comparator interrupt enable -> ANALOG_COMP_vect (ACI in ACSR)
- ACI: Analog Comparator interrupt flag
- ACIS1:ACIS0: Comparator interrupt trigger configuration */


/* Two-Wire Interface Control Register: TWCR
----------------------------------------------------------------------------
[TWCR]       [ TWINT TWEA TWSTA TWSTO TWWC TWEN – TWIE ]
----------------------------------------------------------------------------
- TWIE: TWI interrupt enable -> TWI_vect (TWINT in TWCR)
- TWINT: TWI interrupt flag */


/* Store Program Memory Control and Status Register: SPMCSR
----------------------------------------------------------------------------
[SPMCSR]     [ SPMIE RWWSB – RWWSRE BLBSET PGWRT PGERS SPMEN ]
----------------------------------------------------------------------------
- SPMIE: SPM Ready interrupt enable -> SPM_READY_vect */


/* Status Register: SREG
----------------------------------------------------------------------------
[SREG]       [ I T H S V N Z C ]
----------------------------------------------------------------------------
- I: Global interrupt enable -> sei() sets I, cli() clears I */
