

#
The Global Interrupt Flag is the I bit in SREG, 
    the master switch for all interrupts. 
sei() turns it on
cli() turns it off
#
set interupt for the ??
TIMSK1 – Timer/Counter1 Interrupt Mask Register
TIMSK1 |= (1 << OCIE1A);

// TIMSK1 is the Timer/Counter1 Interrupt Mask Register.
// bit:    7   6    5     4   3     2       1       0
//       [ – | – | ICIE1 | – | – | OCIE1B | OCIE1A | TOIE1 ]
/*
    ICIE1	Input Capture	TIMER1_CAPT_vect	0x0014
    OCIE1A	Compare Match A	TIMER1_COMPA_vect	0x0016
    OCIE1B	Compare Match B	TIMER1_COMPB_vect	0x0018
*/

When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the
Timer/Counter1 Output Compare A Match interrupt is enabled. The corresponding Interrupt Vector (see
“Interrupts” on page 66) is executed when the OCF1A Flag, located in TIFR1, is set.



# USCR0A
7: (RXC0) - Receive Complete
6: (TXC0) - Transmit Complete
5: (UDRE0) - USART Data Register Empty
4: FE0 - Frame Error
3: DOR0 - Data OverRun
2: UPE0 - USART Parity Error
1: U2X0 - Double USART Transmission Speed
0: MPCM0 - Multi-processor Communication Mode

 - RXC0: becomes 1 when unread data is available in the receive buffer.
 - TXC0: becomes 1 when the entire frame has been transmitted, including the stop bit.
 - UDRE0: becomes 1 when the transmit data register can accept another byte.
 - FE0: indicates that a received frame has an invalid stop bit.
 - DOR0: indicates that data was lost because the receiver couldn't process incoming data quickly enough.
 - UPE0: indicates a parity error when parity checking is enabled.
 - U2X0: enables double-speed asynchronous mode, changing the baud-rate calculation.
 - MPCM0: enables a special mode for multiprocessor communication.

# USCR0B:
7: RXCIE0 - Receive Complete Interrupt Enable
6: TXCIE0 - Transmit Complete Interrupt Enable
5: UDRIE0 - USART Data Register Empty Interrupt Enable
4: RXEN0 - Receiver Enable
3: TXEN0 - Transmitter Enable
2: UCSZ02 - USART Character Size bit 2
1: RXB80 - Receive Data bit 8
0: TXB80 - Transmit Data bit 8

 - RXEN0: set to 1 to enable the UART receiver.
 - TXEN0: set to 1 to enable the UART transmitter.
 - RXCIE0: enables an interrupt when a byte has been received.
 - TXCIE0: enables an interrupt when transmission is complete.
 - UDRIE0: enables an interrupt when the transmit data register becomes empty.
 - UCSZ02: combines with two bits in UCSR0C to select the character size.
 - RXB80 and TXB80: used for the ninth data bit when operating in 9-bit mode

# USCR0C:
7: UMSEL01 - USART Mode Select bit 1
6: UMSEL00 - USART Mode Select bit 0
5: UPM01 - USART Parity Mode bit 1
4: UPM00 - USART Parity Mode bit 0
3: USBS0 - USART Stop Bit Select
2: UCSZ01 - USART Character Size bit 1
1: UCSZ00 - USART Character Size bit 0
0: UCPOL0 - USART Clock Polarity

What they control

1. UMSEL01:0 — communication mode

00: Asynchronous USART (ordinary UART)
01: Synchronous USART
10: Reserved
11: Master SPI mode

2. UPM01:0 — parity (Parity is an optional error-detection bit.)

00: Disabled
10: Even parity
11: Odd parity
01: Reserved

3. USBS0 — stop bits

0 = 1 stop bit
1 = 2 stop bits

4. UCSZ01:0 — character size

These two bits combine with UCSZ02 in UCSR0B to select the data size.

UCSZ02:0    Data bits

000         5
001         6
010         7
011         8
111         9

5. UCPOL0 — clock polarity

Controls the clock relationship in synchronous mode. For ordinary asynchronous UART, this bit is ignored