#include <util/delay.h>

#include "src/rgb.h"
#include "src/logic.h"
#include "src/uart.h"


char    g_input_buffer[SIZE_MAX_BUFFER_INTPUT];
uint8_t g_is_username_correct;

/*/////////////////////////////////////////////////////////////////////////////]
You must make a program that listen to the serial port.
When you send a new line with an HEX RGB color with the format #RRGGBB sets the
color on the RGB LED on pin D5.

Gamma correction: out = 255 × (in/255)^γ, with γ ≈ 2.2. 
    In practice, you precompute a 256-entry lookup table, ideally in flash with PROGMEM, 
    because the chip has no fast floating point. Low values get squeezed and high values get spread out, 
    so equal steps in the input look like equal steps in brightness.
                                                                              */
/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    uart_init();
    init_rgb();
    set_luminosity(100);

    while (1) {
        
        if (get_input("Please enter a color in '#RRGGBB format: ", 0))
            handle_color();
        else 
        {
            uart_printstr("some error happened, please retry.\r\n");
            continue;
        }
    }
}
