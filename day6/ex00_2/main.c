#include <avr/io.h>

#include "src/i2c.h"
#include "src/uart.h"

/*/////////////////////////////////////////////////////////////////////////////]
• The AVR ATmega328P microcontroller has 1 I2C peripheral that you must use in
this exercise to communicate with an AHT20 temperature sensor (U3).
• You must write a function i2c_init that initializes the I2C on the microcontroller.
• The MCU’s I2C must be configured so that the communication frequency is 100kHz.
• You must write a function i2c_start that starts an I2C transmission between the
microcontroller and the sensor.
• The program must return status values to your computer after each data transmis-
sion.
• You must write a function i2c_stop that interrupts communication between the
microcontroller and the sensor
*//////////////////////////////////////////////////////////////////////////////]

// util/delay.h is not authorized here: wait with Timer1 instead
// CTC mode (WGM12), prescaler 1024 (CS12 | CS10) -> 16MHz / 1024 = 15625 ticks/s
static void wait_1s(void)
{
    TCNT1 = 0;
    OCR1A = 15625 - 1;
    TIFR1 = _BV(OCF1A); // clear the flag (write 1)
    TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10);

    do {} while (IS_BIT_OFF(TIFR1, OCF1A));
    TCCR1B = 0; // stop the timer
}

int main( void )
{
    uart_init();
    i2c_init();

    while (1)
    {
        // also covers the AHT20 power-on delay (>= 40ms) on the first loop
        wait_1s();

        i2c_start();
        i2c_stop();
        uart_printstr(" ---\r\n");
    }
}
