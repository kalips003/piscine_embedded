#include <stdint.h>
#include <util/delay.h>
#include "src/rgb.h"


static const t_rgb pattern[] = {
        {1,0,0},
        {0,1,0},
        {0,0,1},
        {1,1,0},
        {0,1,1},
        {1,0,1},
        {1,1,1}
};

#define PATTERN_COUNT (sizeof(pattern) / sizeof(pattern[0]))
/*/////////////////////////////////////////////////////////////////////////////]
• The LED should turn on in red, then green, then blue in a loop.
• Changing every 1 second
                                                                              */
/////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    SET_OUTPUT_RGB;

    while (1)
    {
        for (uint8_t i = 0; i < PATTERN_COUNT; ++i)
        {
            TURN_ON_RGB(pattern[i].r, pattern[i].g, pattern[i].b);
            _delay_ms(1000);
        }
    }
}
