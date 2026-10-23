#include "i2c.h"
#include "uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
*//////////////////////////////////////////////////////////////////////////////]
static void i2c_print_status(void)
{
    char buffer[3] = {0};
    write_hexa(I2C_STATUS, buffer);

    uart_printstr("status from AHT20: 0x");
    uart_printstr(buffer);
    uart_printstr("\r\n");
}

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

/*/////////////////////////////////////////////////////////////////////////////]
    START, then SLA+W: the AHT20 answers ACK (0x18) if it is there
*//////////////////////////////////////////////////////////////////////////////]
void i2c_start(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTA: Send a START when the bus is free
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    I2C_WAIT;
    i2c_print_status(); // 0x08 TW_START

    // load SLA+W, then send it (TWSTA cleared by this write)
    TWDR = START_WRITTING(AHT20_ADDR);
    TWCR = _BV(TWINT) | _BV(TWEN);
    I2C_WAIT;
    i2c_print_status(); // 0x18 TW_MT_SLA_ACK / 0x20 TW_MT_SLA_NACK
}

/*/////////////////////////////////////////////////////////////////////////////]
*//////////////////////////////////////////////////////////////////////////////]
void i2c_stop(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTO: Send a STOP, hardware clears it when done
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}
