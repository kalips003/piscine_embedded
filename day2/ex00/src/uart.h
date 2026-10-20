#pragma once

/* view uart_block_diagram.png */

#include <avr/io.h>

#include "lib.h"

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
    #define UART_BAUDRATE 115200UL
#endif


void uart_init( void );
void uart_tx(char c);


#define ENABLE_UART do { \
            UDR0 = 0; /* READ / WRITE BUFFER */ \
            UBRR0 = UART_BAUDRATE; /* + FORMULA */ \
            UCSR0B[TXEN0] = 1; /* ENABLE TRANSMITING */ \
            UCSR0B[RXEN0] = 1; /* ENABLE RECIEVEING */ \
            UCSR0C = 0; /* dont touch, default already 8N1 */ \
        } while (0)


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # USART I/O Data Register 0
----------------------------------------------------------------
[UDR0] : RXB7 - RXB6 - RXB5 - RXB4 - RXB3 - RXB2 - RXB1 - RXB0
----------------------------------------------------------------
	Holds the data byte. Writing sends a byte (transmit buffer), reading gets
	the received byte (receive buffer). One address, two separate buffers.

* TXB/RXB[7:0] : the 8 data bits of the byte being sent or received
*//////////////////////////////////////////////////////////////////////////////]



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # USART Baud Rate Register 0 High
----------------------------------------------------------------
[UBRR0]: 12-bit baud rate value = [UBRR0H]3:0 = UBRR0[11:8] + [UBRR0L]7:0 UBRR0[7:0]
----------------------------------------------------------------
	! writing UBRR0L updates the baud rate immediately.

	Normal async: UBRR = F_CPU / (16 × baud) − 1
	Double async: UBRR = F_CPU / (8 × baud) − 1
*//////////////////////////////////////////////////////////////////////////////]
    // BAUD = F_CPU / (16 * (UBRR0 + 1));
    // UBRR0 = F_CPU / (16 * BAUD) - 1;
    // err = (115200 - BAUD) * 100 / 115200;
//  for : 115200 
// > 16*(8+1) = 111111 (-3.5%)
// > 8*(16+1) = 117647 (+2.1%)
#define BAUD_115200 16

// For 8N1 the datasheet recommends staying within about ±2%
// The (+ half the divisor) rounds to the nearest instead of truncating (16.36 -> 16, 7.68 -> 8)
#define UBRR_NORMAL(baud) ((F_CPU + 8UL * (baud)) / (16UL * (baud)) - 1)
#define UBRR_DOUBLE(baud) ((F_CPU + 4UL * (baud)) / (8UL * (baud)) - 1)

// set correct baud rate in UBRR0, and set speed in U2X0
#define SET_BAUDRATE_DOUBLE(baud) do { SET_PIN(UCSR0A, U2X0); UBRR0 = UBRR_DOUBLE(baud); } while (0)
#define SET_BAUDRATE_NORMAL(baud) do { UNSET_PIN(UCSR0A, U2X0); UBRR0 = UBRR_NORMAL(baud); } while (0)



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
    # USART Control And Status Register 0 A
----------------------------------------------------------------------------
[UCSR0A] : RXC0 - TXC0 - UDRE0 - FE0 - DOR0 - UPE0 - U2X0 - MPCM0
----------------------------------------------------------------------------
	Mostly status flags: data received, transmission done, buffer ready,
	and the error flags. Plus two mode bits (U2X0, MPCM0).

* RXC0  : Receive Complete, set when there is an unread byte in UDR0
* TXC0  : Transmit Complete, set when the whole frame has been shifted out and
          the buffer is empty. Cleared by writing 1 to it or when its interrupt runs
* UDRE0 : Data Register Empty, set when UDR0 can accept a new byte to send
* (FE0) : Frame Error, the stop bit of the received byte was 0 (often a wrong baud rate)
* (DOR0): Data OverRun, a byte was lost because the receive buffer was full
* (UPE0): USART Parity Error, the received byte failed the parity check
* U2X0  : Double Speed, async only. Divides by 8 instead of 16, doubling the speed
* MPCM0 : Multi-processor Communication Mode, ignores frames that are not addresses
*//////////////////////////////////////////////////////////////////////////////]

// RXC0: set when receive something
#define UART_HAS_DATA    (UCSR0A & _BV(RXC0))
#define UART_WAIT_DATA   do { } while (!UART_HAS_DATA)
// UDRE0: set when transmit finished
#define UART_CAN_SEND    (UCSR0A & _BV(UDRE0))
#define UART_WAIT_SEND   do { } while (!UART_CAN_SEND)
// RXC0: set when transmit finished, connection can close
#define UART_SENT        (UCSR0A & _BV(TXC0))

#define UART_SOME_ERR_HAPPENED (UCSR0A & (_BV(FE0)|_BV(DOR0)|_BV(UPE0)))



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
USART Control And Status Register 0 B
----------------------------------------------------------------------------
[UCSR0B] : RXCIE0 - TXCIE0 - UDRIE0 - RXEN0 - TXEN0 - UCSZ02 - RXB80 - TXB80
----------------------------------------------------------------------------
	Turns the receiver/transmitter and their interrupts on or off.
	Also holds the 9th data bit when using 9-bit frames.

* RXCIE0 : RX Complete Interrupt Enable, interrupt when a byte is received (RXC0)
* TXCIE0 : TX Complete Interrupt Enable, interrupt when transmission is done (TXC0)
* UDRIE0 : Data Register Empty Interrupt Enable, interrupt when UDR0 is ready (UDRE0)
* RXEN0  : Receiver Enable, turns on the receiver, RXD0 pin becomes the input
* TXEN0  : Transmitter Enable, turns on the transmitter, TXD0 pin becomes the output
*(UCSZ02): Character Size bit 2, combined with UCSZ01:00 in UCSR0C (1 = 9-bit mode)
* RXB80  : Receive Data Bit 8, the 9th bit received. Read it BEFORE UDR0
* TXB80  : Transmit Data Bit 8, the 9th bit to send. Write it BEFORE UDR0
*//////////////////////////////////////////////////////////////////////////////]

// TXEN0: Transmitter enable: PD1 stops being a normal pin, it becomes TXD
#define UART_TX_ON   (UCSR0B |= _BV(TXEN0))
#define UART_TX_OFF  (UCSR0B &= ~_BV(TXEN0))
// RXEN0: Receiver enable: PD0 stops being a normal pin, it becomes RXD
#define UART_RX_ON   (UCSR0B |= _BV(RXEN0))
#define UART_RX_OFF  (UCSR0B &= ~_BV(RXEN0))



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////]
USART Control And Status Register 0 C
----------------------------------------------------------------------------
[UCSR0C] : UMSEL01 - UMSEL00 - UPM01 - UPM00 - USBS0 - UCSZ01 - UCSZ00 - UCPOL0
----------------------------------------------------------------------------
	Sets the frame format: mode, parity, stop bits and data size.

* UMSEL0[1:0]: USART Mode Select
               [00] = asynchronous, [01] = synchronous, [10] = reserved, [11] = Master SPI
* UPM0[1:0]  : Parity Mode
               [00] = disabled, [01] = reserved, [10] = even, [11] = odd
* USBS0      : Stop Bit Select, [0] = 1 stop bit, [1] = 2 stop bits
* UCSZ0[1:0]: Character Size, together with UCSZ02
               [000] = 5 bits, [001] = 6, [010] = 7, [011] = 8, [111] = 9
* UCPOL0     : Clock Polarity, sync mode only. Picks which XCK0 edge
               sends and samples data. Write 0 in async mode

The default after reset (UCSR0C = 0x06 [0b0110]) is already the classic 8N1: async, no parity, 1 stop bit, 8 data bits
*//////////////////////////////////////////////////////////////////////////////]

// Set UsartModeSelect UMSEL (UCSR0C[7:6])
#define SET_UMSEL(b1, b0) \
    (UCSR0C = (UCSR0C & ~(0b11000000)) | ((b1<<UMSEL01)|(b0<<UMSEL00)))

// Set UsartParityMode UPM (UCSR0C[5:4])
// parity only detects 1 flipped bit, and only tells you (UPE0), it fixes nothing
#define SET_UPM(b1, b0) \
    (UCSR0C = (UCSR0C & ~(0b00110000)) | ((b1<<UPM01)|(b0<<UPM00)))

// Set UsartStopBitSelect USBS (UCSR0C[3])
#define SET_USBS(b0) (UCSR0C = (UCSR0C & ~_BV(USBS0))|(b0<<USBS0))

// Set UsartCharacterSiZe UCSZ across UCSR0B[2] & UCSR0C[2:1]
#define SET_UCSZ(b2, b1, b0) do { \
            UCSR0B = (UCSR0B & ~_BV(UCSZ02))|(b2<<UCSZ02); \
            UCSR0C = (UCSR0C & ~(_BV(UCSZ01)|_BV(UCSZ00)))|((b1<<UCSZ01)|(b0<<UCSZ00)); \
        } while (0)

// UCPOL0: clock polarity, synchronous mode only. Keep it 0 in asynchronous mode.
#define SET_FRAME_8N1 do { SET_UMSEL(0,0); SET_UPM(0,0); SET_USBS(0); SET_UCSZ(0,1,1); } while (0)



/*/////////////////////////////////////////////////////////////////////////////]
On the PC side:
    screen /dev/ttyUSB0 115200          (quit: ctrl+a then k then y)
    The terminal must use the same baud + 8N1, or you get garbage characters.

    - pressing Enter sends '\r' (13), not '\n'
    - to go to the next line you must send "\r\n": '\n' goes down, '\r' goes back left
    - screen doesn't echo what you type: if you want to see it, the MCU must send it back
    - backspace sends 127 (DEL) or 8 ('\b')

*//////////////////////////////////////////////////////////////////////////////]
