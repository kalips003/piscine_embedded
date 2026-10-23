#pragma once

#include <avr/io.h>

#include "lib.h"

#ifndef UART_BAUDRATE
    #define UART_BAUDRATE 115200UL
#endif


void uart_init( void );
void uart_tx(char c);
void uart_printstr(const char* str);


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # USART Baud Rate Register 0
----------------------------------------------------------------
[UBRR0]: 12-bit baud rate value = [UBRR0H]3:0 = UBRR0[11:8] + [UBRR0L]7:0 UBRR0[7:0]
----------------------------------------------------------------
	! writing UBRR0L updates the baud rate immediately.

	Normal async: UBRR = F_CPU / (16 × baud) − 1
	Double async: UBRR = F_CPU / (8 × baud) − 1
*//////////////////////////////////////////////////////////////////////////////]
// for 115200:
// > 16*(8+1) = 111111 (-3.5%)
// > 8*(16+1) = 117647 (+2.1%)
// For 8N1 the datasheet recommends staying within about ±2%
// The (+ half the divisor) rounds to the nearest instead of truncating (16.36 -> 16)
#define UBRR_DOUBLE(baud) ((F_CPU + 4UL * (baud)) / (8UL * (baud)) - 1)

// set correct baud rate in UBRR0, and set speed in U2X0
#define SET_BAUDRATE_DOUBLE(baud) do { SET_PIN(UCSR0A, U2X0); UBRR0 = UBRR_DOUBLE(baud); } while (0)



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # USART Control And Status Register 0 A
----------------------------------------------------------------------------
[UCSR0A] : RXC0 - TXC0 - UDRE0 - FE0 - DOR0 - UPE0 - U2X0 - MPCM0
----------------------------------------------------------------------------
* UDRE0 : Data Register Empty, set when UDR0 can accept a new byte to send
* U2X0  : Double Speed, async only. Divides by 8 instead of 16, doubling the speed
*//////////////////////////////////////////////////////////////////////////////]
#define UART_CAN_SEND    (UCSR0A & _BV(UDRE0))
#define UART_WAIT_SEND   do { } while (!UART_CAN_SEND)



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
USART Control And Status Register 0 B
----------------------------------------------------------------------------
[UCSR0B] : RXCIE0 - TXCIE0 - UDRIE0 - RXEN0 - TXEN0 - UCSZ02 - RXB80 - TXB80
----------------------------------------------------------------------------
* TXEN0  : Transmitter Enable, turns on the transmitter, TXD0 pin becomes the output
*(UCSZ02): Character Size bit 2, combined with UCSZ01:00 in UCSR0C (1 = 9-bit mode)
*//////////////////////////////////////////////////////////////////////////////]
// TXEN0: Transmitter enable: PD1 stops being a normal pin, it becomes TXD
#define UART_TX_ON   (UCSR0B |= _BV(TXEN0))



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
USART Control And Status Register 0 C
----------------------------------------------------------------------------
[UCSR0C] : UMSEL01 - UMSEL00 - UPM01 - UPM00 - USBS0 - UCSZ01 - UCSZ00 - UCPOL0
----------------------------------------------------------------------------
* UMSEL0[1:0]: USART Mode Select, [00] = asynchronous
* UPM0[1:0]  : Parity Mode, [00] = disabled
* USBS0      : Stop Bit Select, [0] = 1 stop bit
* UCSZ0[1:0] : Character Size, together with UCSZ02, [011] = 8 bits
*//////////////////////////////////////////////////////////////////////////////]
#define SET_UMSEL(b1, b0) \
    (UCSR0C = (UCSR0C & ~(0b11000000)) | ((b1<<UMSEL01)|(b0<<UMSEL00)))
#define SET_UPM(b1, b0) \
    (UCSR0C = (UCSR0C & ~(0b00110000)) | ((b1<<UPM01)|(b0<<UPM00)))
#define SET_USBS(b0) (UCSR0C = (UCSR0C & ~_BV(USBS0))|(b0<<USBS0))
#define SET_UCSZ(b2, b1, b0) do { \
            UCSR0B = (UCSR0B & ~_BV(UCSZ02))|(b2<<UCSZ02); \
            UCSR0C = (UCSR0C & ~(_BV(UCSZ01)|_BV(UCSZ00)))|((b1<<UCSZ01)|(b0<<UCSZ00)); \
        } while (0)

#define SET_FRAME_8N1 do { SET_UMSEL(0,0); SET_UPM(0,0); SET_USBS(0); SET_UCSZ(0,1,1); } while (0)
