#pragma once

#include <avr/io.h>

#include "lib.h"

// RGB_R = PD5
// RGB_G = PD6
// RGB_B = PD3
#define SET_OUTPUT_RGB (DDRD |= (_BV(PD5) | _BV(PD6) | _BV(PD3)))
#define TURN_ON_ALL_RGB (PORTD |= (_BV(PD5) | _BV(PD6) | _BV(PD3)))

#define TURN_ON_RGB(r,g,b) (PORTD = (PORTD & ~(_BV(PD5) | _BV(PD6) | _BV(PD3))) \
    | (!!(r) << PD5) | (!!(g) << PD6) | (!!(b) << PD3))
