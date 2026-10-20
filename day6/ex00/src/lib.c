#include "lib.h"

/*/////////////////////////////////////////////////////////////////////////////]

- SPMCSR — Store Program Memory Control and Status Register. You set SIGRD (bit 5) and SPMEN (bit 0) to enable signature-row reading.
- Z — a 16-bit pointer formed by AVR registers R31:R30. It holds the signature-row byte address.
- LPM — Load Program Memory instruction. With signature-row reading enabled, it reads the selected byte into a CPU register.
The automotive ATmega328P datasheet explicitly lists TSOFFSET at 0x0002 and TSGAIN at 0x0003

// asm volatile (
//     "assembly instructions"
//     : outputs
//     : inputs
// );
*//////////////////////////////////////////////////////////////////////////////]
uint8_t read_signature_byte(uint16_t address)
{
    uint8_t value;

    while (SPMCSR & (1 << SPMEN))
        ; // Wait until signature/program-memory operation is ready

    asm volatile (
        "out %[spmcsr], %[control]" "\n\t"
        "lpm %[value], Z"
        : [value] "=r" (value)
        : [spmcsr] "I" (_SFR_IO_ADDR(SPMCSR)),
          [control] "r" ((1 << SIGRD) | (1 << SPMEN)),
          "z" (address)
    );
    return value;
}


static const char hex_base[] = "0123456789abcdef";
static const char dec_base[] = "0123456789";

void write_hexa(uint8_t num, char buff[2])
{
    buff[0] = hex_base[(num & 0xF0) >> 4];
    buff[1] = hex_base[num & 0x0F];
}

char* itoa(int16_t num, char* buff)
{
    uint8_t len = 1;
    int16_t tmp = num;

    if (tmp < 0) {
        *buff = '-';
        buff++;
        tmp = -tmp;
    }

    while (tmp >= 10) {
        tmp /= 10;
        len++;
    }

    tmp = len;
    do {
        buff[--len] = dec_base[num % 10];
        num /= 10;
    } while (num != 0);

    return buff + tmp;
}

