#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"

void init(void) {

   		/* estet‰‰n kaikki keskeytykset */
		cli();

        /* kaiutin pinnit ulostuloksi */
        DDRE  |=  (1 << PE4) | (1 << PE5);
        /* pinni PE4 nollataan */
        PORTE &= ~(1 << PE4);
        /* pinni PE5 asetetaan */
        PORTE |=  (1 << PE5);   
        
        /* ajastin nollautuu, kun sen ja OCR1A rekisterin arvot ovat samat */
        TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
        TCCR1B |=    (1 << WGM12);
        TCCR1B &=   ~(1 << WGM13);

        /* salli keskeytys, jos ajastimen ja OCR1A rekisterin arvot ovat samat */
        TIMSK |= (1 << OCIE1A);

        /* asetetaan OCR1A rekisterin arvoksi 0x3e (~250hz) */
        OCR1AH = 0x00;
        OCR1AL = 0x13;

        /* k‰ynnist‰ ajastin ja k‰yt‰ kellotaajuutena (16 000 000 / 1024) Hz */
        TCCR1B |= (1 << CS12) | (1 << CS10);

		/* n‰pp‰in pinnit sis‰‰ntuloksi */
		DDRA &= ~(1 << PA0);
		DDRA &= ~(1 << PA2);
		DDRA &= ~(1 << PA4);

		/* rele/led pinni ulostuloksi */
		DDRA |= (1 << PA6);

		/* lcd-n‰ytˆn alustaminen */
		lcd_init();
		lcd_write_ctrl(LCD_ON);
		lcd_write_ctrl(LCD_CLEAR);


}

int main(void) 
{

		/* alusta laitteen komponentit */
		init();

		/* alustetaan muuttujat */
		char nimi[5] = {'j', 'a', 'n', 'n', 'e'};
		int i = 0;	/* silmukkamuuttuja */
					
        /* ikuinen silmukka */
        while (1) {
		/* TƒMƒN IKUISEN SILMUKAN SISƒLLƒ OLEVAN KOODIN OPISKELIJAT TEKEVƒT ITSE */
	    	if (!(PINA & (1 << PA4))) {
				PORTA |= (1 << PA6);
			} else {
				PORTA &= (0 << PA6);
			} 
			if (!(PINA & (1 << PA2))) {
				for (; i <= 4; i++) {
					lcd_write_data(nimi[i]);
				}
			} else {
				lcd_write_ctrl(LCD_CLEAR);
				i = 0;
			}	
		  	if (!(PINA & (1 << PA0))) {
				sei();	
			} else {
				cli();
			}
				
		}
}

ISR(TIMER1_COMPA_vect) {

	/* vaihdetaan kaiutin pinnien tilat XOR operaatiolla */
 	PORTE ^= (1 << PE4) | (1 << PE5); 
}

