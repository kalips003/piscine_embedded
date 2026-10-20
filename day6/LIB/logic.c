#include "logic.h"

#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "lib.h"
#include "uart.h"
#include "rgb.h"

void handle_username()
{
    if (!strcmp(g_input_buffer, USER)) // match
        g_is_username_correct = 1;
    else
        g_is_username_correct = 0;
}

void handle_color()
{
    set_rgb_hex(g_input_buffer);
}


void handle_password()
{
    if (!strcmp(g_input_buffer, PASSWORD)) // match
    {
        uart_printstr("Welcome!\r\n");
        leds_routine_1();
    }
    else
        uart_printstr("Not Welcome!\r\n");
}


static const uint8_t patterns[] = {
    0b1111,
    0b0111,
    0b0011,
    0b0001,
    0b0000,    
    0b0001,
    0b0011,
    0b0111
};

#define PATTERN_COUNT (sizeof(patterns) / sizeof(patterns[0]))
#define PATTERN_LOOP 10

void leds_routine_1(void) 
{
    for (uint8_t i = 0; i < PATTERN_COUNT * PATTERN_LOOP; ++i)
    {
        // if (!(i%PATTERN_COUNT))
        //     uart_tx(i/PATTERN_COUNT+65);
        SET_LEDS(patterns[i % PATTERN_COUNT]);
        _delay_ms(100);

    } /* do while(1) */
    SET_LEDS(0);

}


// read an entire line until \n, store in g_input_buffer
// mode 0 = echo; 1 = echo *
int get_input( const char* msg, uint8_t mode )
{
    uart_printstr(msg);

    char    c;
    uint8_t i = 0;
    while (i < SIZE_MAX_BUFFER_INTPUT - 1) 
    {
        c = uart_rx();
        if (c == '\n' || c == '\r')
        {
            uart_printstr("\r\n");
            g_input_buffer[i] = '\0';
            return 1; /*success*/
        }
        else if (c == '\0')
        {
            uart_printstr("\r\n");
            g_input_buffer[i] = '\0';
            return 0; /* 0x00 = user sent NUL (ctrl+@) or a line break condition; real rx errors are in UCSR0A */
        }
        else if (c == 0x7F) /* || c == '\b' */
        {
            if (i > 0)
            {
                --i;
                uart_printstr("\b \b");
            }
            continue;
        }
        else if (c < 32)
            continue;
        g_input_buffer[i++] = c;
        if (!mode)
            uart_tx(c);
        else
            uart_tx('*');
    }
    uart_printstr("\r\nDo you know the price of RAM?!?, stop sleeping on your keyboard!\r\n");
    return 0;
}