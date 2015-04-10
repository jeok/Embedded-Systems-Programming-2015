# Embedded-Systems-Programming-2015
Obligatory coursework for course [521142A](https://weboodi.oulu.fi/oodi/opintjakstied.jsp?MD5avain=&Kieli=1&OpinKohd=16913373&OnkoIlmKelp=1&takaisin=ilmsuor.jsp&haettuOrg=-1&sortJarj=2&Kieli=1&NimiTunniste=embedded&AlkPvm=&PaatPvm=&Selite=&Sivu=0&haettuOpas=-1&haettuOppAin=&haettuLk=-1&haettuOpetKiel=-1&haeOpetTap=haeopetustapahtumat&haeVainIlmKelp=0&haeMyosAlemOrg=1&eHOPSopinkohtlaj=&eHOPSpaluusivu=&eHOPSilmsuor=1) at the University of Oulu.

A car game for AVR ATmega128.

##Dependencies:
  * avr-libc
  * avr-gcc
  * avr-gdb
  * avrdude (for loading to device)

## Compiling
### Manually
  1. avr-gcc -w -Os -DF_CPU=16000000UL -mmcu=atmega128 -c main.c lcd.c
  2. avr-gcc -w -mmcu=atmega128 *.o -o main
  3. avr-objcopy -O ihex -R .eeprom main main.hex

### Using Makefile
  1. make hex

### Load to AVR
 Assuming you use the AVR-JTAG-USB-programmer (and it is connected to /dev/ttyUSB0).
  1. sudo avrdude -F -V -c jtag1 -p ATmega128 -U flash:w:main.hex -P /dev/ttyUSB0
 
 Or with Makefile:
  1. sudo make flash
