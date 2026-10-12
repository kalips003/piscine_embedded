#include "src/uart.h"
// #include <util/delay.h>

/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    while (1) {
        char c = uart_rx();
        // _delay_ms(500);
        uart_tx(c);
    }
}