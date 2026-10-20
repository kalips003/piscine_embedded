#include <util/delay.h>


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
int main( void )
{

}



/*/////////////////////////////////////////////////////////////////////////////]
                                NOTES
• RV1 must be able to change the color of D5 with the Wheel function.
• Also the LEDs D1-D4 must display the value of RV1 as a digital gauge.
◦ LED D1: 25%
◦ LED D2: 50%
◦ LED D3: 75%
◦ LED D4: 100%
*//////////////////////////////////////////////////////////////////////////////]