#pragma once

#include <avr/io.h>

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

uint8_t read_signature_byte(uint16_t address);
void write_hexa(uint8_t num, char buff[2]);
char* itoa2(int16_t num, char* buff);


#define SET_WRITING_PIN(register, pin) (register |= _BV(pin))
#define SET_PIN(register, pin) (register |= _BV(pin))
#define SET_READING_PIN(register, pin) (register &= ~_BV(pin))
#define UNSET_PIN(register, pin) (register &= ~_BV(pin))
#define READ_PIN(register, pin) ((register & _BV(pin)) != 0)
#define TURN_ON_PIN(register, pin) (register |= _BV(pin))
#define TURN_OFF_PIN(register, pin) (register &= ~_BV(pin))
#define TOOGLE_PIN(register, pin) (register ^= _BV(pin))
#define IS_BIT_ON(register, pin) !!(register & _BV(pin))
#define IS_BIT_OFF(register, pin) !(register & _BV(pin))

// LED_D[1:4] on PB0 / PB1 / PB2 / PB4
// set output
#define SET_OUTPUT_LED1 SET_WRITING_PIN(DDRB, PB0) // LED_D1
#define SET_OUTPUT_LED2 SET_WRITING_PIN(DDRB, PB1) // LED_D2
#define SET_OUTPUT_LED3 SET_WRITING_PIN(DDRB, PB2) // LED_D3
#define SET_OUTPUT_LED4 SET_WRITING_PIN(DDRB, PB4) // LED_D4
#define SET_OUTPUT_ALL_LEDS (DDRB |= 0b00010111) // ready the 4 leds
// turn on / off
#define TURN_ON_LED1 TURN_ON_PIN(PORTB, PB0) // LED1
#define TURN_OFF_LED1 TURN_OFF_PIN(PORTB, PB0)
#define TURN_ON_LED2 TURN_ON_PIN(PORTB, PB1) // LED2
#define TURN_OFF_LED2 TURN_OFF_PIN(PORTB, PB1)
#define TURN_ON_LED3 TURN_ON_PIN(PORTB, PB2) // LED3
#define TURN_OFF_LED3 TURN_OFF_PIN(PORTB, PB2)
#define TURN_ON_LED4 TURN_ON_PIN(PORTB, PB4) // LED4
#define TURN_OFF_LED4 TURN_OFF_PIN(PORTB, PB4)
#define TURN_OFF_ALL_LEDS (PORTB &= ~0b00010111) // turn off all leds
#define SET_LEDS(p)      (PORTB = (PORTB & ~0b00010111) | ((p) & 0x07) | (((p) & 0x08) << 1))

// button SW1 on PD2
// button SW2 on PD4
#define SET_READING_SW1 SET_READING_PIN(DDRD, PD2) 
#define SET_READING_SW2 SET_READING_PIN(DDRD, PD4) 
/* SET_READING_SWx sets the pin as an input but doesn't turn on the internal pull-up resistor. 
    That's fine if the board has its own external pull-ups. 
    If it doesn't, also set PORTDn = 1; */

// Buttons states
// SW1 is active-low: idle = 1, pressed = 0
#define SW1_NOT_PRESSED READ_PIN(PIND, PD2)
#define SW1_PRESSED (!READ_PIN(PIND, PD2))
#define SW2_NOT_PRESSED READ_PIN(PIND, PD4)
#define SW2_PRESSED (!READ_PIN(PIND, PD4))
