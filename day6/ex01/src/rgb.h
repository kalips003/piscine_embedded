#pragma once

#include <avr/io.h>
#include <stdint.h>

#include "lib.h"
#include "timer.h"

typedef struct s_rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}   t_rgb;


void init_rgb();
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
void wheel(uint8_t pos);
void set_rgb_hex(const char* hex);
void set_luminosity(uint8_t lum);

// RGB_R = PD5(OC0B/T1)
// RGB_G = PD6(OC0A/AIN0)
// RGB_B = PD3(OC2B/INT1)
#define SET_OUTPUT_RGB (DDRD |= (_BV(PD5) | _BV(PD6) | _BV(PD3)))
#define TURN_ON_ALL_RGB (PORTD |= (_BV(PD5) | _BV(PD6) | _BV(PD3)))

#define TURN_ON_RGB(r,g,b) (PORTD = (PORTD & ~(_BV(PD5) | _BV(PD6) | _BV(PD3))) \
    | (!!(r) << PD5) | (!!(g) << PD6) | (!!(b) << PD3))
