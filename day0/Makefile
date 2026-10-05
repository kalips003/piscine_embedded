
all: hex flash
	echo hello

# compile main.c > main.bin (bin for the AVR/Amega326p)
# compile main.bin > main.hex (intel HEX)
# -O ihex → output format = Intel HEX
hex: main.c
	echo hex
	avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL main.c -o main.bin
	avr-objcopy -O ihex main.bin main.hex

# send main.hex > microcontroller
# avrdude (speaks the AVR programming protocol)
# Arduino programmer (hardware/interface between your PC and the ATmega328P)

# -p (part) m328p → target ATmega328P
# -c (programmer type) arduino → communicate using the Arduino bootloader protocol
# -P (port) /dev/ttyUSB0 → the serial device corresponding to your USB connection
# -b (baud rate) 115200 → baud rate specified by your piscine
# -U (memory operation) flash:w:main.hex → write (w) main.hex to flash

# ls -l /dev/serial/by-id/
BOARD = /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0

flash: main.hex
	avrdude -p m328p -c arduino -P $(BOARD) -b 115200 -U flash:w:main.hex

# delete main.bin, main.hex
clean:
	rm -f main.bin main.hex

.PHONY: all hex flash clean