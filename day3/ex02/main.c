#include <stdint.h>
#include <util/delay.h>
#include "src/rgb.h"

#define DELAY_WHEEL 20
/*/////////////////////////////////////////////////////////////////////////////]
Ok, now even more colors! You must write a program that controls the RGB LED
D5 but with PWM. Make the colors transition from one to the next smoothly using the
provided wheel function.
• Write a function init_rgb() that initializes the timers.
• Then write a function set_rgb() that allows you to set the color of the LED.
• Finally, your program must display the color wheel.
The colors obtained in the requested gradient are always the sum of
two primary colors, no more, sometimes less.

Gamma correction: out = 255 × (in/255)^γ, with γ ≈ 2.2. 
    In practice, you precompute a 256-entry lookup table, ideally in flash with PROGMEM, 
    because the chip has no fast floating point. Low values get squeezed and high values get spread out, 
    so equal steps in the input look like equal steps in brightness.
                                                                              */
/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    init_rgb();

    while (1)
    {
        for (uint8_t i = 0; i < 255; ++i)
        {
            wheel(i);
            _delay_ms(DELAY_WHEEL);
        }
    }
}
