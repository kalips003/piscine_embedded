#include "src/uart.h"
#include "src/interrupt.h"

/////////////////////////////////////////////////////////////////////////////]
// == ISR(USART_RX_vect) { /* ... */ }
// set __vector_18 (function called on USART byte received)
void USART_RX_vect(void) __attribute__((__signal__, __used__));
void USART_RX_vect(void)
{
    uint8_t err = UART_SOME_ERR_HAPPENED; // read error before UDR0, else you read the error of the next byte
    char c = UDR0; // read register where the arrived char is stored

    if (err)
        return;

    if (c >= 'a' && c <= 'z')
        c -= 32;
    else if (c >= 'A' && c <= 'Z')
        c += 32;
    uart_tx(c);
}

/////////////////////////////////////////////////////////////////////////////]
// Reset and interupt vector
// TIMSK1 is the Timer/Counter1 Interrupt Mask Register.
    // OCIE1A: 0x0016: TIMER1 COMPA Timer/Counter1 Compare Match A
void interupt_init( void ) 
{
    UART_RX_INT_ON; // enable interrupt for RXCIE0: Receive Complete  -> USART_RX_vect
    sei(); // Set I bit   → interrupts on
}

/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    interupt_init();

    while (1) {}
}

/////////////////////////////////////////////////////////////////////////////]
/*  BETTER, 'CLEANER' VERSION

volatile uint8_t rx_flag = 0;
volatile char    rx_char = 0;
volatile uint8_t rx_err  = 0;

void USART_RX_vect(void) __attribute__((__signal__, __used__));
void USART_RX_vect(void)
{
    rx_err  = UART_RX_ERROR;   // status first
    rx_char = UDR0;            // then data (clears RXC0)
    rx_flag = 1;               // flag last
}

static void handle_echo(void)
{
    char    c;
    uint8_t err;

    cli();                     // copy and clear together, so the ISR can't run in between
    c   = rx_char;
    err = rx_err;
    rx_flag = 0;
    sei();

    if (err)
        return;
    if (c >= 'a' && c <= 'z')
        c -= 32;
    else if (c >= 'A' && c <= 'Z')
        c += 32;
    uart_tx(c);                // the slow part runs with interrupts on
}

while (1) {
    if (rx_flag)
        handle_echo();
}

*/

/////////////////////////////////////////////////////////////////////////////]
/* EVEN BETTER WITH A RING BUFFER:
The ISR writes only head and the main loop writes only tail

#define RX_SIZE 16             // power of 2
volatile char    rx_buf[RX_SIZE];
volatile uint8_t rx_head = 0;  // written by ISR only
volatile uint8_t rx_tail = 0;  // written by main only

void USART_RX_vect(void)
{
    uint8_t err = UART_RX_ERROR;
    char c = UDR0;
    uint8_t next = (rx_head + 1) & (RX_SIZE - 1);
    if (!err && next != rx_tail) {   // drop on error or buffer full
        rx_buf[rx_head] = c;
        rx_head = next;
    }
}

// main loop
while (1) {
    if (rx_tail != rx_head) {
        char c = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) & (RX_SIZE - 1);
        // swap case, uart_tx(c)
    }
}

*/