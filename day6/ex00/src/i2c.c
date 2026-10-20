#include "i2c.h"

/*/////////////////////////////////////////////////////////////////////////////]
*//////////////////////////////////////////////////////////////////////////////]
void i2c_init(void)
{
    // TWI clock prescaler = 1
    TWSR = 0x00; // TWPS1:0 = 00

    // Configure SCL frequency
    TWBR = (uint8_t)((F_CPU / I2C_FREQ - 16) / 2); // 72 at 16 MHz

    // Enable TWI peripheral
    TWCR = (1 << TWEN); // TWEN = 1
}

static void i2c_wait(void)
{
    while (!(TWCR & (1 << TWINT)))
        ;
}

static uint8_t i2c_status(void)
{
    return TWSR & 0xF8; // Mask out TWPS1:0
}

uint8_t i2c_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2c_wait();

    uint8_t status = i2c_status();

    return (status == 0x08 || status == 0x10);
}

uint8_t i2c_write(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN);
    i2c_wait();

    return i2c_status() == 0x28;
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}