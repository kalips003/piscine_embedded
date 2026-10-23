#include "i2c.h"

/*/////////////////////////////////////////////////////////////////////////////]
*//////////////////////////////////////////////////////////////////////////////]
void i2c_init(void)
{
    // TWI clock prescaler = 1
    TWSR = 0x00; // TWPS1:0 = 00

    // Configure SCL frequency (100kHz)
    TWBR = (uint8_t)TWBR_VALUE(I2C_FREQ); // 72 at 16 MHz

    // Enable TWI peripheral: takes over PC4/PC5
    TWCR = _BV(TWEN); // TWEN = 1
}

void i2c_start(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTA: Send a START when the bus is free
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    I2C_WAIT; // 0x08 TW_START
}

void i2c_stop(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTO: Send a STOP, hardware clears it when done
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

/////////////////////////////////////////////////////////////////////////////]
void i2c_write(unsigned char data)
{
    // load the byte (only allowed while TWINT = 1, else TWWC)
    TWDR = data;

    // go: clock out 8 bits, the slave answers on the 9th
    TWCR = _BV(TWINT) | _BV(TWEN);

    I2C_WAIT; // SLA+W 0x18 / SLA+R 0x40 / data 0x28
}

/////////////////////////////////////////////////////////////////////////////]
// last = 1 on the last byte to read (NACK = "that's enough")
uint8_t i2c_read_byte(uint8_t last)
{
    if (!last) // go: clock in 1 byte, then ACK it
        TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
    else // go: clock in 1 byte, then NACK it (last one)
        TWCR = _BV(TWINT) | _BV(TWEN); // & TWEA = 0

    I2C_WAIT; // 0x50 TW_MR_DATA_ACK / 0x58 TW_MR_DATA_NACK

    return TWDR;
}
