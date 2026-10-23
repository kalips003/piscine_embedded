#include "tools.h"
#include <string.h>

static const char hex_base[] = "0123456789ABCDEF";

uint8_t wii_hex(char c)
{
    for (uint8_t h = 0; hex_base[h] != 0; ++h)
    {
        if (hex_base[h] == c)
            return h;
    }
    return 0;
}

uint8_t atoi_base_hex(const char* hex)
{
    return wii_hex(*hex) * 16 + wii_hex(*(hex+1));
}

t_rgb read_rgb_hex(const char* hex)
{
    t_rgb rtrn = {0};
    if (!hex || *hex != '#' || strlen(++hex) != 6)
        return rtrn;

    rtrn.r = atoi_base_hex(hex); hex +=2;
    rtrn.g = atoi_base_hex(hex); hex +=2;
    rtrn.b = atoi_base_hex(hex);

    return rtrn;
}