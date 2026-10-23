#pragma once

#include <avr/io.h>
#include <stdint.h>
#include <util/twi.h>

#include "lib.h"

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

#ifndef I2C_FREQ
    #define I2C_FREQ 100000UL // 100kHz, "standard mode"
#endif


typedef union u_AHT20 {
    struct {
        uint8_t status;
        uint8_t b1, b2, b3, b4, b5;
        uint8_t crc;
    };
    uint8_t raw[7];
} t_AHT20;

typedef struct s_readings {
    uint8_t     status;
    uint32_t    hum;
    uint32_t    temp;
    uint8_t     crc;

}   t_reading;

typedef struct s_values {
    float    temp;
    float    hum;

}   t_values;

void        i2c_init(void);
void        i2c_start(void);
void        i2c_stop(void);
uint8_t     i2c_start_transmit(uint8_t addr, uint8_t read_write);
uint8_t     i2c_write_better(uint8_t data);
uint8_t     i2c_read_better(uint8_t last, uint8_t* data);

void i2c_print_status();
uint8_t i2c_start_better(void);

void i2c_write(unsigned char data);
void i2c_read(void);
void print_hex_value(char c);

void    aht20_readings();
t_reading i2c_read2(void);


/*/////////////////////////////////////////////////////////////////////////////]
TWI = I2C
1 bus line for clock: SCL (master generate clock)
1 bus line for data: SDA
(need pull up resistor)
to enable i2c, the ???[PRTWI] bit must be written 0
7 bit slave adress (0-127)
transmission is initiated when the master issues a START, terminated with a STOP
(exception = REPEATED START)
packets on the TWI bus are 9 bit long (7 for adrr, one r/w, one acknowledge)
if r/w set > read is performed
else > write is performed
ACK = pulling SDA low on the 9th cycle
NACK = leaving SDA high
CPU clock frequency in the slave must be at least 16 times higher than the SCL frequency.

TWBR	Sets the SCL clock frequency divider.
TWSR	Contains status codes and clock prescaler bits.
TWAR	Configures the device's own address when acting as a target.
TWDR	Holds the byte being transmitted or received.
TWCR	Controls the TWI peripheral, including START, STOP, enable and interrupt flags.
*//////////////////////////////////////////////////////////////////////////////]



/*/////////////////////////////////////////////////////////////////////////////]
                                    I2C / TWI

    2 wires, shared by everyone on the bus:
        SDA = PC4 (data)
        SCL = PC5 (clock, always driven by the master)
    Both are open-drain: devices can only pull the line LOW, a pull-up resistor
    brings it back HIGH. Nobody ever drives a 1 -> no short circuit if 2 talk.

    Once TWEN = 1 the TWI hardware owns PC4/PC5, DDRC/PORTC don't matter anymore.

    idle: SDA=1 SCL=1
     START   : SDA goes LOW while SCL is HIGH            (only the master does this)
     ADDRESS : 7 bits address + 1 bit R/W  = "SLA+W" or "SLA+R"
               [A6 A5 A4 A3 A2 A1 A0 R/W]  -> SLA+W = (addr << 1) | 0
                                              SLA+R = (addr << 1) | 1
     ACK     : 9th clock, the receiver pulls SDA LOW = "got it" (ACK)
               SDA left HIGH = NACK (nobody there / no more data please)
     DATA    : 8 bits, MSB first, + ACK/NACK, as many bytes as you want
     STOP    : SDA goes HIGH while SCL is HIGH -> bus free again

     REPEATED START: a START without a STOP before, to change direction
                     (write a register number, then read it) without letting
                     another master grab the bus.

    Rule: SDA only changes while SCL is LOW. SDA changing while SCL is HIGH
    is START or STOP, never data.
*//////////////////////////////////////////////////////////////////////////////]


/*/////////////////////////////////////////////////////////////////////////////]
    # TWI Bit Rate Register
----------------------------------------------------------------------------
[TWBR] : TWBR7 - TWBR6 - TWBR5 - TWBR4 - TWBR3 - TWBR2 - TWBR1 - TWBR0
----------------------------------------------------------------------------
	Sets the SCL frequency (master mode only), with the prescaler in TWSR.

	SCL = F_CPU / (16 + 2 * TWBR * prescaler)
	TWBR = (F_CPU / SCL - 16) / (2 * prescaler)

	16MHz, 100kHz, prescaler 1 -> TWBR = (160 - 16) / 2 = 72
*//////////////////////////////////////////////////////////////////////////////]
#define TWBR_VALUE(freq) (((F_CPU / (freq)) - 16UL) / 2UL) // prescaler = 1
#define I2C_SET_FREQ(freq) (TWBR = TWBR_VALUE(freq))



/*/////////////////////////////////////////////////////////////////////////////]
    # TWI Control Register
----------------------------------------------------------------------------
[TWCR] : TWINT - TWEA - TWSTA - TWSTO - TWWC - TWEN - – - TWIE
----------------------------------------------------------------------------
    Enables TWI and starts operations:
	Every action (start, send, receive, stop) = write TWCR, then wait TWINT.

* TWINT : TWI Interrupt Flag. Set by hardware when the current job is done.
          While TWINT = 1 the SCL line is held LOW (bus frozen, waiting for us).
          Cleared by writing 1 to it (yes, 1) -> that is what starts the next job.
          -> so: set TWDR / the other bits FIRST, write TWINT LAST (same write)
* TWEA  : Enable Acknowledge. When receiving: [1] = send ACK after the byte
          ("give me more"), [0] = send NACK ("that was the last one")
* TWSTA : START condition. Send a START when the bus is free.
          Must be cleared by software after the START went out.
* TWSTO : STOP condition. Send a STOP, hardware clears it when done.
          (TWINT is NOT set after a STOP, don't wait for it)
* TWWC  : Write Collision, you wrote TWDR while TWINT was 0 (too early)
* TWEN  : TWI Enable, takes over PC4/PC5. [0] = off, kills any transfer
* TWIE  : TWI Interrupt Enable -> TWI_vect while TWINT = 1

	! use  TWCR = ...  not  TWCR |= ...
	  |= keeps old TWSTA / TWEA bits from the previous step
*//////////////////////////////////////////////////////////////////////////////]
#define I2C_WAIT        do {} while (IS_BIT_OFF(TWCR, TWINT))

#define I2C_SEND_START  (TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN))
#define I2C_SEND_STOP   (TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN))
#define I2C_WAIT_STOP   do {} while (IS_BIT_ON(TWCR, TWSTO))
// send what is in TWDR
#define I2C_SEND        (TWCR = _BV(TWINT) | _BV(TWEN))
// receive one byte into TWDR, then ACK (more) or NACK (last byte)
#define I2C_RECV_ACK    (TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA))
#define I2C_RECV_NACK   (TWCR = _BV(TWINT) | _BV(TWEN))



/*/////////////////////////////////////////////////////////////////////////////]
    # TWI Status Register
----------------------------------------------------------------------------
[TWSR] : TWS7 - TWS6 - TWS5 - TWS4 - TWS3 - – - TWPS1 - TWPS0
----------------------------------------------------------------------------
    Status codes and clock prescaler
	What just happened on the bus (valid when TWINT = 1), + the prescaler.

* TWS[7:3]  : status code. Mask out the prescaler bits: TWSR & 0xF8
              (util/twi.h gives TW_STATUS which does exactly that)
* TWPS[1:0] : Bit rate prescaler
               [00] = 1    [01] = 4    [10] = 16    [11] = 64

	Status codes (util/twi.h names), master mode:
	    code  name                  meaning
	    0x08  TW_START              START sent
	    0x10  TW_REP_START          repeated START sent
	  transmitter (after SLA+W):
	    0x18  TW_MT_SLA_ACK         SLA+W sent, slave said ACK  (it's there)
	    0x20  TW_MT_SLA_NACK        SLA+W sent, NACK            (nobody / busy)
	    0x28  TW_MT_DATA_ACK        data byte sent, ACK
	    0x30  TW_MT_DATA_NACK       data byte sent, NACK
	    0x38  TW_MT_ARB_LOST        lost the bus to another master
	  receiver (after SLA+R):
	    0x40  TW_MR_SLA_ACK         SLA+R sent, ACK
	    0x48  TW_MR_SLA_NACK        SLA+R sent, NACK
	    0x50  TW_MR_DATA_ACK        byte received, we sent ACK
	    0x58  TW_MR_DATA_NACK       byte received, we sent NACK
	  misc:
	    0xF8  TW_NO_INFO            nothing to report (TWINT = 0)
	    0x00  TW_BUS_ERROR          illegal START/STOP on the bus
*//////////////////////////////////////////////////////////////////////////////]
#define I2C_STATUS (TWSR & 0xF8) // Mask out TWPS1:0
#define I2C_SET_PRESCALER(b1, b0) (TWSR = (TWSR & ~0b11) | ((b1<<TWPS1)|(b0<<TWPS0)))


/*/////////////////////////////////////////////////////////////////////////////]
    # TWI Data Register
----------------------------------------------------------------------------
[TWDR] : TWD7 - TWD6 - TWD5 - TWD4 - TWD3 - TWD2 - TWD1 - TWD0
----------------------------------------------------------------------------
    Holds transmitted or received bytes

	Transmit: the next byte to send (address SLA+R/W or data).
	Receive : the last byte received.
	Only writable while TWINT = 1 (else TWWC is set and nothing happens).
    TWD0:
        0 = Write (Master → Slave)
        1 = Read (Slave → Master)
*//////////////////////////////////////////////////////////////////////////////]
#define START_WRITTING(addr) (((addr) << 1) | TW_WRITE)
#define START_READING(addr) (((addr) << 1) | TW_READ)



/*/////////////////////////////////////////////////////////////////////////////]
    # TWI (Slave) Address Register / Address Mask
----------------------------------------------------------------------------
[TWAR]  : TWA6 - TWA5 - TWA4 - TWA3 - TWA2 - TWA1 - TWA0 - TWGCE
[TWAMR] : TWAM[6:0] - –
----------------------------------------------------------------------------
	Only used when the MCU is a SLAVE (its own address). Master = ignore.
* TWGCE : answer the General Call address 0x00
*//////////////////////////////////////////////////////////////////////////////]



/*/////////////////////////////////////////////////////////////////////////////]
                                    AHT20 (U3)
    7-bit address: 0x38  -> SLA+W = 0x70, SLA+R = 0x71

    Power on: wait >= 40ms before talking to it.

    Status byte (read: START, SLA+R, read 1 byte):
        bit 7 : Busy, [1] = measuring
        bit 3 : CAL enable, [1] = calibrated
        -> if bit 3 = 0: send init 0xBE 0x08 0x00 then wait 10ms

    Measure:
        1. START, SLA+W, 0xAC 0x33 0x00, STOP     (trigger measurement)
        2. wait >= 80ms (or poll the busy bit until it is 0)
        3. START, SLA+R, read 7 bytes (ACK x6, NACK on the last), STOP
             [0] status
             [1] humidity    [19:12]
             [2] humidity    [11:4]
             [3] humidity [3:0] | temperature [19:16]
             [4] temperature [15:8]
             [5] temperature [7:0]
             [6] CRC-8 (poly 0x31, init 0xFF) over bytes [0..5]

        RH[%] = S_rh / 2^20 * 100
        T[C]  = S_t  / 2^20 * 200 - 50
        (resolution 0.024%RH / 0.01C, accuracy +-2%RH / +-0.3C)

    Don't measure too often (self-heating): ~1 measure every 2s max
    (check "measurement frequency" in the AHT20 datasheet)
*//////////////////////////////////////////////////////////////////////////////]
#define AHT20_ADDR          0x38
#define AHT20_CMD_INIT      0xBE // + 0x08 0x00
#define AHT20_CMD_TRIGGER   0xAC // + 0x33 0x00
#define AHT20_CMD_RESET     0xBA // soft reset, wait 20ms after
#define AHT20_BUSY(status)  IS_BIT_ON(status, 7)
#define AHT20_CAL(status)   IS_BIT_ON(status, 3)
