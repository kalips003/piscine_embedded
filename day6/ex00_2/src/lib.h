#pragma once

#include <avr/io.h>

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

void write_hexa(uint8_t num, char buff[2]);

#define SET_PIN(register, pin) (register |= _BV(pin))
#define UNSET_PIN(register, pin) (register &= ~_BV(pin))
#define IS_BIT_ON(register, pin) !!(register & _BV(pin))
#define IS_BIT_OFF(register, pin) !(register & _BV(pin))
