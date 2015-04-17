
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include "lcd.h"


/* The Fast & Furious: AVR ATmega128 */
/* Written by Janne Körkkö and Valtteri Talvensaari */

/* Global variables for buttons and gamestate */
volatile int button_0, button_1, button_2, button_3, button_4, gamestate, track_progress;

/* Macros */
#define LEN(a) ((sizeof(a) / sizeof(*a))-1) // remove 1, because only used in loops

/* Function prototypes */
void print_rows(char row_1[], char row_2[], int row_1_len, int row_2_len);
void render(char road_table[2][16], int car_x, int car_y, int car_jumping);
void handle_input();
void main_menu();
void update_game(char road_table[2][16], int *car_x, int *car_y, int *car_jumping, uint16_t *score);
void update_car(int *car_x, int *car_y, int *car_jumping);
void game();
void scores();
void delay(int ms);


void init(void) {

   		/* disable interrupts */
		cli();

        /* speaker pins to output */
        DDRE  |=  (1 << PE4) | (1 << PE5);
        /* reset pin PE4 */
        PORTE &= ~(1 << PE4);
        /* set pin PE5 */
        PORTE |=  (1 << PE5);   
        
        /* timer resets when it has the same value as OCR1A */
        TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
        TCCR1B |=    (1 << WGM12);
        TCCR1B &=   ~(1 << WGM13);

        /* enable interrupt if OCR1A register holds the same values */
        TIMSK |= (1 << OCIE1A);

        /* set OCR1A to be 15625 */
        //OCR1AH = 0x3d;
        //OCR1AL = 0x09;
		OCR1A = 10000;

        /* käynnistä ajastin ja käytä kellotaajuutena (16 000 000 / 1024) Hz */
        TCCR1B |= (1 << CS12) | (1 << CS10);

		/* button pins to input */
		DDRA &= ~(1 << PA0);
		DDRA &= ~(1 << PA2);
		DDRA &= ~(1 << PA4);

		/* led pin to output */
		DDRA |= (1 << PA6);

		/* initialize lcd */
		lcd_init();
		lcd_write_ctrl(LCD_ON);
		lcd_write_ctrl(LCD_CLEAR);

		/* Check for valid content in EEPROM. If valid data not found, initialize memory slot */
		uint16_t eeprom_data = eeprom_read_word((uint16_t*)1);
		if (eeprom_data == -1) {
			uint16_t init_data = 0;
			eeprom_update_word((uint16_t*)1, init_data);
		}

}


int main(void)
{	
	/* Initialize pins and ports */
    init();
	gamestate = 1;
	
	while(1) {
		cli();
		if (gamestate == 1) { 
			main_menu();
		} 
		if (gamestate == 2) {
			track_progress = 0;
			game();
		} 
		if (gamestate == 3) {
			scores();
		}
	}	
	
	return 0;
}


/* Function definitions */

void print_rows(char row_1[], char row_2[], int row_1_len, int row_2_len)
{
	/* Print given rows */
	lcd_write_ctrl(LCD_CLEAR);


	int a = 0;
	for (; a < row_1_len; a++) {
		lcd_write_data(row_1[a]);
	}
	a = 0;
	for (; a < 40 - row_1_len; a++) {
		lcd_write_ctrl(LCD_SHIFT_R);
	}
	a = 0;
	for (; a < row_2_len; a++) {
		lcd_write_data(row_2[a]);
	}
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
	} if (!(PINA & (1 << PA1))) { 
		button_1 = 1; 
	} if (!(PINA & (1 << PA2))) {
		button_2 = 1; 
	} if (!(PINA & (1 << PA3))) {
		button_3 = 1; 
	} if (!(PINA & (1 << PA4))) {
		button_4 = 1;
	}
}

void main_menu()
{	
	/* Display text for main menu */
	char menu_text_1[] = "Fast & Furious";
	char menu_text_2[] = "<HiScore< >Play>";

	print_rows(menu_text_1, menu_text_2, LEN(menu_text_1), LEN(menu_text_2));
	
	/* Loop for user input and break from loop */
	while(1) 
	{
		handle_input();
		if (button_3 == 1) 
		{
			gamestate = 2;
			srand((TCNT1));
			break;
		} 
		else if (button_1 == 1) 
		{
			gamestate = 3;
			break;
		}
	}	
		
}

void game()
{

    /* Declare variables */
	int car_x = 15;
	int car_y = 1;
	int car_jumping = 0;
	uint16_t score = 0;
	char road[2][16] = {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
						{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

	OCR1A = 10000;
	sei(); /* Enable interruptions */
	
	while(1) 
	{
		handle_input();
		update_car(&car_x, &car_y, &car_jumping);
		delay(50); /* Display can't keep up without drawing delay */
		update_game(road, &car_x, &car_y, &car_jumping, &score);
		render(road, car_x, car_y, car_jumping);


		if (gamestate == 1)
		{
			char row_1[] = "Game Over!";
			char row_2[13] = "score: ";
			char str[6];
			snprintf(str, 6, "%05d", score);
			strcat(row_2, str);
			print_rows(row_1, row_2, LEN(row_1), LEN(row_2));

			// Save hi-score to EEPROM
			uint16_t eeprom_data = eeprom_read_word((uint16_t*)1);
			if (eeprom_data < score) {
				eeprom_update_word((uint16_t*)1, score);	
			}
			score = 0;
			delay(2000);
			break;
		}

	}
}

void scores()
{
	/* Display the highscore */
	uint16_t highscore = eeprom_read_word((uint16_t*)1);
	char row_1[] = "Hi-Score:";
	char row_2[6];
	snprintf(row_2, 6, "%05d", highscore);
	print_rows(row_1, row_2, LEN(row_1), LEN(row_2));


	gamestate = 1; /* Go back to main menu */
	delay(2000);
}

void render(char road_table[2][16], int car_x, int car_y, int car_jumping)
{
	/* Make a printable version of road and insert car on its place */
	char printable_road[2][16];
	int x = 0;

	for (; x < 16 ; x++)
	{
		printable_road[0][x] = road_table[0][x];
	}
	x = 0;
	for (; x < 16 ; x++)
	{
		printable_road[1][x] = road_table[1][x];
	}
	if (car_jumping > 0)
	{
		printable_road[car_y][car_x] = 'O';
	}
	else
	{
		printable_road[car_y][car_x] = 'o';
	}


	print_rows(printable_road[0], printable_road[1], 16, 16);
}



void update_game(char road_table[2][16], int *car_x, int *car_y, int *car_jumping, uint16_t *score)
{
	if(track_progress)
	{
		/* Update road_table so that every tile gets the symbol from the tile in front of it,
			starting from the end. */
		int x = 15;
		for (; x > 0; x--)
		{
			road_table[0][x] = road_table[0][x-1];
		}
		x = 15;
		for (; x > 0; x--)
		{
			road_table[1][x] = road_table[1][x-1];
		}


		/* clear first tiles */
		road_table[0][0] = ' ';
		road_table[1][0] = ' ';


		/* generate new obstacles */

		int boosters = rand() %15;
		if (boosters == 1)
		{
			road_table[0][0] = '<';
		}
		else if (boosters == 2)
		{
			road_table[1][0] = '<';
		}

		int gates = rand() % 7;
		if (road_table[0][1] == ' ' && road_table[1][1] == ' ' && 
			 road_table[0][2] == ' ' && road_table[1][2] == ' ')
		{
			/* If there are no preceding gates, spawn two gates */
			if ((gates) == 0)
			{	
				if(!(road_table[0][3] == ']' && road_table[1][3] == '[')) /* To avoid too much double gates */
				{	
					road_table[0][0] = ']';
					road_table[1][0] = ']';
				}
			} 
			/* If there is one preceding gate */
			else if ((gates) == 1)
			{
				road_table[0][0] = ']';
			}
			/* If there are two preceding gates */
			else if ((gates) == 2)
			{
				road_table[1][0] = ']';
			}
		}

		/* If player hits an arrow, decrease timer => increase speed */
		if (road_table[*car_y][*car_x] == '<')
		{
			if(OCR1A > 1000)
				{OCR1A = OCR1A -500;}
		}
		/* If car is in air, decrease car_jumping by one. */
		if (*car_jumping > -1) 
		{
			*car_jumping += -1;  
		}
		*score += 1;
		track_progress = 0;
	}
	/* check for death */
	if (road_table[*car_y][*car_x] == ']' && *car_jumping <= 0)
	{
		gamestate = 1;
	}
}

void update_car(int *car_x, int *car_y, int *car_jumping)
{
	/* Update car position or state of jumping on user input */
	if (button_0 == 1)
	{
		*car_y = 0;
	}
	else if (button_4 == 1)
	{
		*car_y = 1;
	}
	
	if (button_2 == 1 && *car_jumping == -1)
		*car_jumping = 4; /* how many ticks car stays on air */
}

void delay(int ms)
{
	for (int i = 0; i < ms; i++)
	{
		_delay_ms(1);
	}
}

ISR(TIMER1_COMPA_vect)
{ 
	/* Interrupt routine for track progress */
	track_progress = 1;
} 
