#include <util/delay.h>
#include "src/rgb.h"

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
        TURN_ON_RGB(1,0,0);
        _delay_ms(1000);
        TURN_ON_RGB(0,1,0);
        _delay_ms(1000);
        TURN_ON_RGB(0,0,1);
        _delay_ms(1000);
    }
}
