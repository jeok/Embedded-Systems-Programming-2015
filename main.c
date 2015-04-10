#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* The Fast & Furious: AVR ATmega128 */
/* Written by Janne Körkkö and Valtteri Talvensaari */

void init(void)
{
    /* Disable interrupts */
    cli();
    /* Set speaker pins to output: */
    /* DDRE |= (1 << PA4) | (1 << PA5); */
    /* Reset pin PE4 */
    PORTE &= ~(1 << PE4);
    /* Set pin PE5 */
    PORTE |= (1 << PE5);

	/* Set button pins to input */
	DDRA &= ~(1 << PA0);
	DDRA &= ~(1 << PA1);
	DDRA &= ~(1 << PA2);
	DDRA &= ~(1 << PA3);
	DDRA &= ~(1 << PA4);

	/* Initialize lcd screen */
	lcd_init();
	lcd_write_ctrl(LCD_ON);
	lcd_write_ctrl(LCD_CLEAR);

}

int main(void)
{
	init();

	/* Declare variables */
	int car_pos_x = 0;
	int car_pos_y = 0;
	//char road[2][16];
	char text[13] = {'F', 'a', 's', 't', 'n', 'F', 'u', 'r', 'i', 'o', 'u', 's'};
	int i = 0; /* Loop variable */
    while(1) {
	/* Loop in main menu, break on user input */
	while (1) {
            for (; i < 12; i++) {
                lcd_write_data(text[i]);
                }
            }

    }

    return 0;
}
