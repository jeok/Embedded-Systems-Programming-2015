help:
	@echo 'Help details:'
	@echo 'hex: compile hex file'
	@echo 'flash: install hex file'
	@echo 'program: compile hex and install'
	
hex:
	avr-gcc -w -Os -DF_CPU=16000000UL -mmcu=atmega128 -c main.c lcd.c
	avr-gcc -w -mmcu=atmega128 *.o -o main
	avr-objcopy -O ihex -R .eeprom main main.hex

flash:
	avrdude -F -V -c jtag1 -p ATmega128 -U flash:w:main.hex -P /dev/ttyUSB0

clean:
	rm *.o
	rm *.hex
