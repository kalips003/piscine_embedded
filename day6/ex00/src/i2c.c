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
    i2c_start_better();
    i2c_print_status();
    i2c_start_transmit(AHT20_ADDR, 1);
    i2c_print_status();
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

uint8_t i2c_start_better(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTA: Send a START when the bus is free
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    I2C_WAIT;

    uint8_t status = I2C_STATUS;

    //              TW_START            TW_REP_START
    return (status == 0x08 || status == 0x10);
}

void i2c_stop(void)
{
    // TWINT: clear twi interrupt flag
    // TWSTO: Send a STOP, hardware clears it when done
    // TWEN: Enable TWI peripheral
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

//                                         1 |   0
uint8_t i2c_start_transmit(uint8_t addr, uint8_t read_write)
{
    // load addr
    if (read_write)
        TWDR = START_WRITTING(addr);
    else
        TWDR = START_READING(addr);

    // send it
    TWCR = _BV(TWINT) | _BV(TWEN);

    I2C_WAIT;
    uint8_t status = I2C_STATUS;

    if (status == TW_MT_SLA_NACK || status == TW_MR_SLA_NACK)
        return 0;
    else 
    // if (status == TW_MT_SLA_ACK || status == TW_MR_SLA_ACK)
        return 1;
}

// send 1 data byte (after SLA+W), the slave ACKs or NACKs it
uint8_t    i2c_write_better(uint8_t data)
{
    // load the byte (only allowed while TWINT = 1, else TWWC)
    TWDR = data;

    // go: clock out 8 bits, the slave answers on the 9th
    TWCR = _BV(TWINT) | _BV(TWEN);

    I2C_WAIT;

    // 0x28: ACK, slave took it / 0x30: NACK, slave refused it (send a STOP)
    return (I2C_STATUS == TW_MT_DATA_ACK);
}

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
    TWDR = data;
}
void i2c_read(void)
{

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