#include "uart.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////]
/* USARTC Universal Synchronous Asynchronous Receiver Transmiter Control */
void uart_init( void )
{
// set correct baud rate in UBRR0, and set double speed
    BAUD_RATE_115200;

// UCSR0C: 8N1 mode: UMSEL0 [Asynchronous], UPM0 [parity disable], USBS0 [1 stop bit], UCSZ0 [011 = 8bit]
    SET_FRAME_8N1;

/* TXEN0: Transmitter enable: PD1 stops being a normal pin, it becomes TXD */
    UART_TX_ON;
// RXEN0: Receiver enable: PD0 stops being a normal pin, it becomes RXD
    UART_RX_ON;
}


/////////////////////////////////////////////////////////////////////////////]
/* send one char over uart */
void uart_tx(char c) 
{
// UCSR0A[UDRE0] = 1: UDR0 is empty, you can write
// UCSR0A[UDRE0] = 0: busy, wait
    UART_WAIT_SEND;

// UDR0 = write register
    UDR0 = c;
}

/* read one char on the uart */
char uart_rx( void )
{
    char    c;
    uint8_t err;

    UART_WAIT_DATA;
    err = UART_RX_ERROR;
    c = UDR0;

    if (err /* error case */)
        return '\0';

    return c;
}