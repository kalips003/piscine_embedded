#include "lib.h"

static const char hex_base[] = "0123456789ABCDEF";

void write_hexa(uint8_t num, char buff[2])
{
    buff[0] = hex_base[(num & 0xF0) >> 4];
    buff[1] = hex_base[num & 0x0F];
}
