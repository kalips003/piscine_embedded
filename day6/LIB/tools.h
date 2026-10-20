#pragma once

#include "rgb.h"

uint8_t wii_hex(char c);
uint8_t atoi_base_hex2(const char* hex);
t_rgb read_rgb_hex(const char* hex);