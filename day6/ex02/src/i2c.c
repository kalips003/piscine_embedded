#include "i2c.h"
#include "lib.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

#include "uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
*//////////////////////////////////////////////////////////////////////////////]
void i2c_init(void)
{
    // TWI clock prescaler = 1
    TWSR = 0x00; // TWPS1:0 = 00

    // Configure SCL frequency (100kHz)
    TWBR = (uint8_t)((F_CPU / I2C_FREQ - 16) / 2); // 72 at 16 MHz

    // Enable TWI peripheral: takes over PC4/PC5
    TWCR = (1 << TWEN); // TWEN = 1
}

void i2c_start(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    I2C_WAIT;
    // i2c_print_status();
}

void i2c_print_status()
{
    char buffer[3] = {0};
    uint8_t status = I2C_STATUS;
    write_hexa(status, buffer);

    uart_printstr("status from AHT20: 0x");
    uart_printstr(buffer);
    uart_printstr("\r\n");
}

void i2c_stop(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTO: Send a STOP, hardware clears it when done
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

void print_hex_value(char c)
{
    char buff[3] = {0};
    write_hexa(c, buff);
    uart_printstr(buff);

}

/////////////////////////////////////////////////////////////////////////////]
// the AHT20 sends a CRC as its 7th byte: to know the data is valid.
uint8_t    i2c_read_better(uint8_t last, uint8_t* data)
{

   
    if (!last) // go: clock in 1 byte, then ACK it
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);   
    else // go: clock in 1 byte, then NACK it (last one)
        TWCR = _BV(TWINT) | _BV(TWEN); // & TWEA = 0

    I2C_WAIT;

    uint8_t expected = last ? TW_MR_DATA_NACK : TW_MR_DATA_ACK;   // 0x58 : 0x50
    *data = TWDR;

    // error: (0x38 lost the bus, 0x00 bus error)
    return (I2C_STATUS == expected);
}


void i2c_write(unsigned char data)
{
    // load the byte (only allowed while TWINT = 1, else TWWC)
    TWDR = data;

    // go: clock out 8 bits, the slave answers on the 9th
    TWCR = _BV(TWINT) | _BV(TWEN);

    I2C_WAIT;
    // i2c_print_status();
}

void i2c_read(void)
{
    uint8_t data;
    for (uint8_t i = 0; i < 7; ++i) {
        i2c_read_better(i == 6, &data);   // NACK the 7th
        print_hex_value(data);
        uart_printstr(" ");
    }
    uart_printstr("\r\n");
}

/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
byte:    [0]        [1]        [2]        [3]         [4]        [5]        [6]
       SSSSSSSS | HHHHHHHH   HHHHHHHH   HHHH TTTT | TTTTTTTT   TTTTTTTT | CCCCCCCC
        status  |  hum 19:12  hum 11:4   hum 3:0    temp 15:8  temp 7:0 |   CRC
                |                        temp 19:16                       |
                |←──── humidity: 20 bits ────→|←──── temperature: 20 bits ─→|

2^20 = 1048576
humidity: 0 → 0 %, 2^20 → 100 %
temperature: 0 → -50 °C, 2^20 → +150 °C (a 200 °C span starting at -50)
*//////////////////////////////////////////////////////////////////////////////]
t_reading i2c_read2(void)
{
    t_AHT20 raw_bits;
    for (uint8_t i = 0; i < 7; ++i) {
        i2c_read_better(i == 6, &raw_bits.raw[i]);   // NACK the 7th
    }

    t_reading rtrn;
    rtrn.status = raw_bits.status;
    rtrn.crc = raw_bits.crc;
    rtrn.hum = (uint32_t)raw_bits.b1 << 12 | (uint32_t)raw_bits.b2 << 4 | (raw_bits.b3 >> 4);
    rtrn.temp = ((uint32_t)raw_bits.b3 & 0b1111) << 16 | (uint32_t)raw_bits.b4 << 8 | (uint32_t)raw_bits.b5;

    return rtrn;
}

#include <stdlib.h>

// Temperature: 17.C, Humidity: 43,4%
void    aht20_readings()
{

    t_reading   untranslated = i2c_read2();
    t_values    values;

    values.hum  = (float)untranslated.hum  / 1048576.0 * 100.0;          // %RH
    values.temp = (float)untranslated.temp / 1048576.0 * 200.0 - 50.0;   // °C

    uart_printstr("Temperature: ");
    char buf[12];                    // sign + 3 digits + '.' + decimals + '\0', with room to spare
    dtostrf(values.temp, 0, 2, buf);    // width 0 = no padding
    uart_printstr(buf);
    uart_printstr(".C, Humidity: ");
    dtostrf(values.hum, 0, 2, buf);    // width 0 = no padding
    uart_printstr(buf);
    uart_printstr("%\r\n");
}
/*
WRITE (trigger measurement)
  START                 → 0x08
  0x70  (SLA+W)         → 0x18  sensor said "I'm here"
  0xAC                  → 0x28  ACK
  0x33                  → 0x28  ACK
  0x00                  → 0x28  ACK
  STOP                     (no TWINT, no status)

  wait ~80ms  (the sensor is measuring; the bus is free in the meantime)

READ (get the 7 bytes)
  START                 → 0x08
  0x71  (SLA+R)         → 0x40  sensor will talk
  read + ACK  ×6        → 0x50  each one; the byte is in TWDR
  read + NACK ×1        → 0x58  last byte (the CRC)
  STOP

*/