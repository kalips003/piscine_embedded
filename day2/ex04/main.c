#include "src/lib.h"
#include "src/uart.h"
#include "src/logic.h"

// #include <stdint.h>
#include <stdint.h>
#include <string.h>

char    g_input_buffer[SIZE_MAX_BUFFER_INTPUT];
uint8_t g_is_username_correct;

/*/////////////////////////////////////////////////////////////////////////////]

• Create 2 strings, a username and a password.
• Display a prompt on the serial port that asks for the username and password.
• When typing the username, there should be an echo.
• same for the password but with ’*’ instead of characters.
• The Backspace key deletes a character.
• The Enter key validates the input.
• If the username and password are correct, the program displays the welcome text
and makes the LEDs blink                                                        */

/////////////////////////////////////////////////////////////////////////////]

// ! A FAIRE • The Backspace key deletes a character.
int main( void )
{
    uart_init();
    SET_OUTPUT_ALL_LEDS;

    while (1) {
        
        if (get_input("Please enter username: ", 0))
            handle_username();
        else 
        {
            uart_printstr("some error happened, please retry.\r\n");
            continue;
        }
        
        if (get_input("Please enter password: ", 1))
            handle_password();
        else 
            uart_printstr("some error happened, please retry.\r\n");
    }
}
