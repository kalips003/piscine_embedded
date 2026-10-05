# 1

main.c
  ↓ compiler
main.bin
  ↓ avr-objcopy
main.hex
  ↓ avrdude
microcontroller's flash memory




# 2
PC
 │
 │ USB
 ▼
Arduino programmer
 │
 │ programming interface
 ▼
AVR microcontroller

# 3


 - ATmega328P → the microcontroller on the devkit.
 - avr-gcc → GCC configured to compile C code for the AVR architecture, rather than your PC's CPU architecture.
 - avr-objcopy → converts the compiled output into the HEX format.
 - avrdude → sends the HEX data to the ATmega328P's flash.
 - Arduino programmer → the programming interface used to communicate with the chip.
 - 115200 baud → the communication speed used by that programmer.
 - ATmega328P datasheet → describes the microcontroller itself: CPU, registers, memory, peripherals, instruction set, electrical characteristics, etc.
 - devkit schematic → describes how the components on your particular board are electrically connected to the ATmega328P.


 # 4 
 flash:
	avrdude [programmer options] -b 115200 ... main.hex

The important pieces we'll determine are:

-p atmega328p → which chip
-c ... → which programmer
-P ... → which USB/serial port
-b 115200 → baud rate
-U flash:w:main.hex → write main.hex to flash