# Embedded-Systems-Programming-2015
Obligatory coursework for course [521142A](https://weboodi.oulu.fi/oodi/opettaptied.jsp?MD5avain=&Kieli=1&OpetTap=35704538&takaisin=ilmsuor.jsp&haettuOrg=-1&sortJarj=2&haettuOrg=-1&Kieli=1&NimiTunniste=embedded&AlkPvm=&PaatPvm=&Selite=&Sivu=0&haettuOpas=-1&haettuOppAin=&haettuLk=-1&haettuOpetKiel=-1&haeOpetTap=haeopetustapahtumat&haeVainIlmKelp=0&haeMyosAlemOrg=1) at the University of Oulu.

A car game for AVR ATmega128.

##Dependencies:
  * avr-libc
  * avr-gcc
  * avr-gdb

## Compiling
  1. avr-gcc -w -Os -DF_CPU=16000000UL -mmcu=atmega128 -c main.c lcd.c
  2. avr-gcc -w -mmcu=atmega128 *.o -o main
  3. avr-objcopy -O ihex -R .eeprom main main.hex
