#pragma once

#include <avr/io.h>
#include <stdint.h>

#include "lib.h"

typedef struct s_rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}   t_rgb;

// RGB_R = PD5
// RGB_G = PD6
// RGB_B = PD3
#define SET_OUTPUT_RGB (DDRD |= (_BV(PD5) | _BV(PD6) | _BV(PD3)))
#define TURN_ON_ALL_RGB (PORTD |= (_BV(PD5) | _BV(PD6) | _BV(PD3)))

#define TURN_ON_RGB(r,g,b) (PORTD = (PORTD & ~(_BV(PD5) | _BV(PD6) | _BV(PD3))) \
    | (!!(r) << PD5) | (!!(g) << PD6) | (!!(b) << PD3))
