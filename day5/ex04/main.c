#include <util/delay.h>

#include "src/analog.h"
#include "src/rgb.h"

// 64-128-192-256
void update_leds(uint8_t value)
{
    // switch off all leds
    TURN_OFF_ALL_LEDS;

    switch (value) {
        case 255:
            TURN_ON_LED4;
            // fall through
        case 192 ... 254:
            TURN_ON_LED3;
            // fall through
        case 128 ... 191:
            TURN_ON_LED2;
            // fall through
        case 64 ... 127:
            TURN_ON_LED1;
            break;
        default:
            break;
    };
}

/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
• RV1 must be able to change the color of D5 with the Wheel function.
• Also the LEDs D1-D4 must display the value of RV1 as a digital gauge.
◦ LED D1: 25%
◦ LED D2: 50%
◦ LED D3: 75%
◦ LED D4: 100%
*//////////////////////////////////////////////////////////////////////////////]
int main( void )
{
    ini_adc();
    init_rgb();
    SET_OUTPUT_ALL_LEDS;

    while (1) {
        uint8_t reading = take_a_reading_8bit();
        wheel(reading);
        update_leds(reading);
        _delay_ms(20);
    }
}
