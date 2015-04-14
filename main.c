#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* The Fast & Furious: AVR ATmega128 */
/* Written by Janne Krkk and Valtteri Talvensaari */

/* Global variables for buttons */
volatile int button_0, button_1, button_2, button_3, button_4;

/* Function prototypes */
void handle_input();

int main(void)
{	
	/* Initialize pins and ports */
    init();

    /* Declare variables */
	/* Char road etc. */	
	
	return 0;
}

/* Function definitions */
void init(void)
{
    /* Disable interrupts */
    cli();
    /* Set speaker pins to output: */
    /* error: stray characters? */
    /* DDRE |= (1 << PA4) |(1 << PA5); */
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


void handle_input() 
{
	/* Check if buttons are pressed and return corresponding integer(s) */	
	/* Initialize buttons */
	button_0 = 0;
	button_1 = 0;
	button_2 = 0;
	button_3 = 0;
	button_4 = 0;

	if (!(PINA & (1 << PA0))) {
		button_0 = 1;  
	} else if (!(PINA & (1 << PA1))) { 
		button_1 = 1; 
	} else if (!(PINA & (1 << PA2))) {
		button_2 = 1; 
	} else if (!(PINA & (1 << PA3))) {
		button_3 = 1; 
	} else if (!(PINA & (1 << PA4))) {
		button_4 = 1;
	}
}

void main_menu()
{	
	/* Print text in main menu */
	int i = 0; /* Loop variable */
	char menu_text[] = "Fast & Furious";
	for (; i >= sizeof(menu_text); i++) {
		lcd_write_data(menu_text[i]);
	}
}

