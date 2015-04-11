## Project flags

PROJECT = main.c lcd.c
OBJECTS = main.o

TARGET = flash.elf
HEX = flash.hex

## Device flags
MCU = atmega128
DF_CPU = 16000000UL
PROGRAMMER = jtagmki
PORT = /dev/ttyUSB0

help:
	@echo 'Help details:'
	@echo 'hex: compile hex file'
	@echo 'flash: install hex file'
	@echo 'program: compile hex and install'
	
hex:
	avr-gcc -w -O2 -DF_CPU=$(DF_CPU) -mmcu=$(MCU) -o $(OBJECTS) $(PROJECT)
	avr-objcopy -O ihex -R .eeprom $(OBJECTS) $(HEX)

flash:
	avrdude -F -V -c $(PROGRAMMER) -p $(MCU) -e -U flash:w:$(HEX) -P $(PORT)

clean:
	rm *.o
	rm *.hex
