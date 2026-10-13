#include "rgb.h"


// RGB_R = PD5(OC0B/T1)
// RGB_G = PD6(OCOA/AIN0)
// RGB_B = PD3(OC2B/INT1)
void init_rgb()
{
    SET_OUTPUT_RGB; 

    OCR0B = 0;
    OCR2B = 0;
    OCR0A = 0;
    SET_COM_T0A(1,0); // COM non-inverting (fast PWM)
    SET_COM_T0B(1,0);
    SET_COM_T2B(1,0);
    SET_WGM_T0(0,1,1); // Fast PWM with TOP
    SET_WGM_T2(0,1,1);
//  start the timers
    SET_CS_T0_PRESCALER_64;
    SET_CS_T2_PRESCALER_64;
}
void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r; /* stop value? */
    OCR2B = g;
    OCR0A = b; /* 1/256? */
}

void wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}