#pragma once

#include <avr/io.h>

#ifndef F_CPU
    #define F_CPU 16000000
#endif

#define SET_WRITING_PIN(register, pin) (register |= _BV(pin))
#define SET_PIN(register, pin) (register |= _BV(pin))
#define SET_READING_PIN(register, pin) (register &= ~_BV(pin))
#define UNSET_PIN(register, pin) (register &= ~_BV(pin))
#define READ_PIN(register, pin) ((register & _BV(pin)) != 0)
#define TURN_ON_PIN(register, pin) (register &= _BV(pin))
#define TURN_OFF_PIN(register, pin) (register &= _BV(pin))

