#pragma once

#include <avr/io.h>
#include <util/twi.h>

#include "lib.h"

#ifndef I2C_FREQ
    #define I2C_FREQ 100000UL // 100kHz, "standard mode"
#endif

void        i2c_init(void);
void        i2c_start(void);
void        i2c_stop(void);

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

	Status codes (util/twi.h names), master transmitter:
	    code  name                  meaning
	    0x08  TW_START              START sent
	    0x10  TW_REP_START          repeated START sent
	    0x18  TW_MT_SLA_ACK         SLA+W sent, slave said ACK  (it's there)
	    0x20  TW_MT_SLA_NACK        SLA+W sent, NACK            (nobody / busy)
	    0x38  TW_MT_ARB_LOST        lost the bus to another master
	    0xF8  TW_NO_INFO            nothing to report (TWINT = 0)
	    0x00  TW_BUS_ERROR          illegal START/STOP on the bus
*//////////////////////////////////////////////////////////////////////////////]
#define I2C_STATUS (TWSR & 0xF8) // Mask out TWPS1:0



/*/////////////////////////////////////////////////////////////////////////////]
    # TWI Data Register
----------------------------------------------------------------------------
[TWDR] : TWD7 - TWD6 - TWD5 - TWD4 - TWD3 - TWD2 - TWD1 - TWD0
----------------------------------------------------------------------------
    Holds transmitted or received bytes

	Transmit: the next byte to send (address SLA+R/W or data).
	Receive : the last byte received.
	Only writable while TWINT = 1 (else TWWC is set and nothing happens).
*//////////////////////////////////////////////////////////////////////////////]
#define START_WRITTING(addr) (((addr) << 1) | TW_WRITE)



/*/////////////////////////////////////////////////////////////////////////////]
                                    AHT20 (U3)
    7-bit address: 0x38  -> SLA+W = 0x70, SLA+R = 0x71
    Power on: wait >= 40ms before talking to it.
*//////////////////////////////////////////////////////////////////////////////]
#define AHT20_ADDR          0x38
