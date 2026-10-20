#include "rgb.h"
// #include "tools.h"


uint8_t luminosity = 255;

// RGB_R = PD5(OC0B/T1)
// RGB_G = PD6(OC0A/AIN0)
// RGB_B = PD3(OC2B/INT1)
void init_rgb()
{
    SET_OUTPUT_RGB; 

    OCR0B = 0;
    OCR2B = 0;
    OCR0A = 0;
// clear on match, set at BOTTOM (non-inverting) (fast PWM)
    T0_SET_COMA(1,0);
    T0_SET_COMB(1,0);
    T2_SET_COMB(1,0);
// Mode 3: Fast PWM - TOP:0xFF - Update:BOTTOM - clear-on:MAX
    T0_SET_WGM(0,1,1);
    T2_SET_WGM(0,1,1);
//  Set prescaler 64 start the timers
    T0_SET_CS(0,1,1);
    T2_SET_CS(0,1,1);
}
void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r * luminosity / 255;
    OCR0A = g * luminosity / 255;
    OCR2B = b * luminosity / 255;
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

// void set_rgb_hex(const char* hex)
// {
//     t_rgb rgb = read_rgb_hex(hex);
//     set_rgb(rgb.r, rgb.g, rgb.b);
// }

void set_luminosity(uint8_t lum)
{
    luminosity = lum;
}