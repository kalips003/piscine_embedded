#pragma once

#include <avr/io.h>
#include <stdint.h>

#ifndef PASSWORD
    #define PASSWORD "mabote"
#endif

#ifndef USER
    #define USER "bibi"
#endif

#define SIZE_MAX_BUFFER_INTPUT 9

// defined in main.c
extern char    g_input_buffer[SIZE_MAX_BUFFER_INTPUT];
extern uint8_t g_is_username_correct;

void handle_username();
void handle_password();
void handle_color();
void set_timer(void);
int get_input( const char* msg, uint8_t mode );
void leds_routine_1(void);

