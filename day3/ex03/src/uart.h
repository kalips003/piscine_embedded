#pragma once

/* Refer to UART.md */

#include <avr/io.h>

#include "lib.h"

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
    #define UART_BAUDRATE 115200UL
#endif


/////////////////////////////////////////////////////////////////////////////]


void uart_init( void );
void uart_tx(char c);
char uart_rx( void );
void uart_printstr(const char* str);


/*/////////////////////////////////////////////////////////////////////////////]
UART = Universal Asynchronous Receiver Transmitter.
    2 wires, one per direction: TX (what I send) and RX (what I receive).
    They are crossed: my TX goes to the other side's RX, and the other way around.
    On the ATmega328P: TXD0 = PD1, RXD0 = PD0
        (on the board they go to the USB-serial chip -> /dev/ttyUSB0 on the PC)

"Asynchronous" = there is no clock wire. Both sides just agree beforehand on:
    - the speed (baud rate = bits per second, ex 115200)
    - the frame format (ex 8N1 = 8 data bits, No parity, 1 stop bit)
    If they don't agree, you receive garbage, not nothing.

The line stays HIGH when nothing happens (idle). One frame (8N1) looks like:

    idle  start  b0  b1  b2  b3  b4  b5  b6  b7  stop  idle
    ‾‾‾‾‾‾|____|‾‾‾‾|___|‾‾‾|___|___|‾‾‾|___|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
           0     \______ data, LSB first ______/   1

    - start bit (always 0): the falling edge tells the receiver "a byte is coming",
        it starts its own timer and samples each bit in its middle
    - data bits: LSB first ('A' = 0x41 = 0b01000001 is sent 1,0,0,0,0,0,1,0)
    - parity bit (optional): an extra bit so the number of 1s is even (or odd)
    - stop bit(s) (always 1): back to idle, gives time before the next start bit
    So 1 byte = 10 bits on the wire: at 115200 baud, ~11520 bytes/s, ~87us per byte.

The USART0 hardware does all of this alone. You only:
    - write a byte into UDR0 -> it gets shifted out on TX
    - read a byte from UDR0  -> the last byte that arrived on RX
    (UDR0 is 2 registers behind one name: a TX buffer when written, a RX buffer when read)
*//////////////////////////////////////////////////////////////////////////////]


/////////////////////////////////////////////////////////////////////////////]
/*/ USART Baud Rate Register: UBRR0[11:0] (UBRR0H[3:0] + UBRR0L[7:0], or UBRR0 directly [0:4095])
    The USART divides F_CPU to get its bit timing.
 - normal speed (U2X0 = 0): UBRR0 = F_CPU / (16 * baud) - 1
 - double speed (U2X0 = 1): UBRR0 = F_CPU / (8 * baud) - 1                      */
// UBRR0 is an integer, so the real baud is never exact:
//      real_baud = F_CPU / (16 or 8 * (UBRR0 + 1))
//      error     = real_baud / wanted_baud - 1
// at 16 MHz:
//      baud    | normal: UBRR  real    error  | double: UBRR  real    error
//      9600    |        103   9615    +0.2%   |        207   9615    +0.2%
//      57600   |         16  58824    +2.1%   |         34  57143    -0.8%
//      115200  |          8 111111    -3.5%   |         16 117647    +2.1%
// For 8N1 the datasheet recommends staying within about ±2%: so 115200 at 16 MHz
//      only works (barely) in double speed. That's why we set U2X0.
// The (+ half the divisor) rounds to the nearest instead of truncating (16.36 -> 16, 7.68 -> 8)
#define UBRR_NORMAL(baud) ((F_CPU + 8UL * (baud)) / (16UL * (baud)) - 1)
#define UBRR_DOUBLE(baud) ((F_CPU + 4UL * (baud)) / (8UL * (baud)) - 1)

#define SET_BAUDRATE_DOUBLE(baud) do { UCSR0A |= _BV(U2X0); UBRR0 = UBRR_DOUBLE(baud); } while (0)
#define SET_BAUDRATE_NORMAL(baud) do { UCSR0A &= ~_BV(U2X0); UBRR0 = UBRR_NORMAL(baud); } while (0)

// set correct baud rate in UBRR0, and set double speed
#define BAUD_RATE_115200 do { SET_PIN(UCSR0A, U2X0); UBRR0 = UBRR_DOUBLE(UART_BAUDRATE); } while (0)

/////////////////////////////////////////////////////////////////////////////]
/*/ Status: UCSR0A: USART control and status register A/B/C
// bit:    7      6      5      4     3      2      1      0
//       [ RXC0 | TXC0 | UDRE0 | FE0 | DOR0 | UPE0 | U2X0 | MPCM0 ]
 - RXC0:  Receive Complete: 1 = an unread byte is waiting in UDR0
            (cleared by reading UDR0)
 - TXC0:  Transmit Complete: 1 = everything is out, stop bit included, nothing left to send
            (cleared by its ISR, or by writing a 1 to it)
 - UDRE0: Data Register Empty: 1 = UDR0 can take a new byte
            (the previous one may still be shifting out, that's fine: it's double buffered)
 - FE0:   Frame Error: the stop bit of the received byte was 0 (wrong baud / format?)
 - DOR0:  Data OverRun: a byte was lost, you didn't read UDR0 fast enough
 - UPE0:  Parity Error: the received parity bit doesn't match
 - U2X0:  Double speed (see baud rate above), the only bit here you set yourself
 - MPCM0: Multi-processor mode (ignore frames that are not addresses), leave it at 0  */
// FE0 / DOR0 / UPE0 are about the byte currently in UDR0: read them BEFORE reading UDR0.
// UDRE0 vs TXC0: UDRE0 = "I can give it another byte", TXC0 = "the line is quiet again"
#define UART_CAN_SEND    (UCSR0A & _BV(UDRE0))
#define UART_BUSY       !(UCSR0A & _BV(UDRE0))
#define UART_HAS_DATA    (UCSR0A & _BV(RXC0))
#define UART_SENT        (UCSR0A & _BV(TXC0))
#define UART_RX_ERROR    (UCSR0A & (_BV(FE0)|_BV(DOR0)|_BV(UPE0)))

#define UART_WAIT_SEND   do { } while (!UART_CAN_SEND)
#define UART_WAIT_DATA   do { } while (!UART_HAS_DATA)


/////////////////////////////////////////////////////////////////////////////]
/*/ Control: UCSR0B: USART control and status register B
// bit:    7        6        5        4       3       2        1       0
//       [ RXCIE0 | TXCIE0 | UDRIE0 | RXEN0 | TXEN0 | UCSZ02 | RXB80 | TXB80 ]
 - RXCIE0 / TXCIE0 / UDRIE0: interrupt enables, see interrupt2.h (UART_xx_INT_ON/OFF)
 - RXEN0: Receiver enable: PD0 stops being a normal pin, it becomes RXD
 - TXEN0: Transmitter enable: PD1 stops being a normal pin, it becomes TXD
            (no need to touch DDRD, the USART takes over the pin)
 - UCSZ02: 3rd bit of the character size (see UCSR0C), only 1 for 9-bit frames
 - RXB80 / TXB80: the 9th data bit in 9-bit mode (read RXB80 before UDR0)          */

/* TXEN0: Transmitter enable: PD1 stops being a normal pin, it becomes TXD */
#define UART_TX_ON   (UCSR0B |= _BV(TXEN0))
#define UART_TX_OFF  (UCSR0B &= ~_BV(TXEN0))
// RXEN0: Receiver enable: PD0 stops being a normal pin, it becomes RXD
#define UART_RX_ON   (UCSR0B |= _BV(RXEN0))
#define UART_RX_OFF  (UCSR0B &= ~_BV(RXEN0))


/////////////////////////////////////////////////////////////////////////////]
/*/ Frame format: UCSR0C: USART control and status register C
// bit:    7         6         5       4       3       2        1        0
//       [ UMSEL01 | UMSEL00 | UPM01 | UPM00 | USBS0 | UCSZ01 | UCSZ00 | UCPOL0 ]
// After reset UCSR0C = 0b00000110 = asynchronous 8N1: already what we want.     */

/*/ Set UsartModeSelect UMSEL (UCSR0C[7:6])
 - [00]: Asynchronous USART (normal UART, no clock wire)
 - [01]: Synchronous USART (clock on XCK0 = PD4)
 - [10]: Reserved
 - [11]: Master SPI (MSPIM)                                                     */
#define SET_UMSEL(b1, b0) \
    (UCSR0C = (UCSR0C & ~(_BV(UMSEL01)|_BV(UMSEL00)))|((b1<<UMSEL01)|(b0<<UMSEL00)))

/*/ Set UsartParityMode UPM (UCSR0C[5:4])
 - [00]: Disabled
 - [01]: Reserved
 - [10]: Even parity (total count of 1s, parity bit included, is even)
 - [11]: Odd parity                                                             */
// parity only detects 1 flipped bit, and only tells you (UPE0), it fixes nothing
#define SET_UPM(b1, b0) \
    (UCSR0C = (UCSR0C & ~(_BV(UPM01)|_BV(UPM00)))|((b1<<UPM01)|(b0<<UPM00)))

/*/ Set UsartStopBitSelect USBS (UCSR0C[3])
 - [0]: 1 stop bit
 - [1]: 2 stop bits (the receiver only checks the first one anyway)             */
#define SET_USBS(b0) (UCSR0C = (UCSR0C & ~_BV(USBS0))|(b0<<USBS0))

/*/ Set UsartCharacterSiZe UCSZ across UCSR0B[2] & UCSR0C[2:1]
 - [000]: 5-bit
 - [001]: 6-bit
 - [010]: 7-bit
 - [011]: 8-bit   <- the usual one
 - [100]: Reserved
 - [101]: Reserved
 - [110]: Reserved
 - [111]: 9-bit (9th bit in TXB80/RXB80)                                        */
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

Full example: 8N1 at 115200, then echo every received byte back.

    int main(void)
    {
        SET_BAUDRATE_DOUBLE(UART_BAUDRATE);
        SET_FRAME_8N1;
        UART_TX_ON;
        UART_RX_ON;
        while (1)
        {
            UART_WAIT_DATA;         // wait for RXC0
            char c = UDR0;          // read it (clears RXC0)
            UART_WAIT_SEND;         // wait for UDRE0
            UDR0 = c;               // send it back
        }
    }
*//////////////////////////////////////////////////////////////////////////////]
