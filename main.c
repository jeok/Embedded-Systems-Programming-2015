
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include "lcd.h"


/* The Fast & Furious: AVR ATmega128 */
/* Written by Janne Körkkö and Valtteri Talvensaari */

/* Global variables for buttons and gamestate */
volatile int button_0, button_1, button_2, button_3, button_4, gamestate;

/* Macros */
/*tfw ollaan vitun juutalaisia */
#define LEN(a) ((sizeof(a) / sizeof(*a))-1)

/* Function prototypes */
void print_rows(char row_1[], char row_2[], int row_1_len, int row_2_len);
void render(char road_table[2][16], int car_x, int car_y, int car_jumping);
void handle_input();
void main_menu();
void update_game(char road_table[2][16], int *car_x, int *car_y, int *car_jumping);
void game();
void scores();



void init(void) {

   		/* estetään kaikki keskeytykset */
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

        /* käynnistä ajastin ja käytä kellotaajuutena (16 000 000 / 1024) Hz */
        TCCR1B |= (1 << CS12) | (1 << CS10);

		/* näppäin pinnit sisääntuloksi */
		DDRA &= ~(1 << PA0);
		DDRA &= ~(1 << PA2);
		DDRA &= ~(1 << PA4);

		/* rele/led pinni ulostuloksi */
		DDRA |= (1 << PA6);

		/* lcd-näytön alustaminen */
		lcd_init();
		lcd_write_ctrl(LCD_ON);
		lcd_write_ctrl(LCD_CLEAR);



}


int main(void)
{	
	/* Initialize pins and ports */
    init();
	gamestate = 1;

	
	while(1) {
		if (gamestate == 1) { 
			main_menu();
		} 
		_delay_ms(500);
		if (gamestate == 2) {
			game();
		} 
		_delay_ms(500);
		if (gamestate == 3) {
			scores();
		}
		_delay_ms(500);
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
	char menu_text_1[] = "Fast & Furious";
	char menu_text_2[] = "<HiScore< >Play>";

	print_rows(menu_text_1, menu_text_2, LEN(menu_text_1), LEN(menu_text_2));

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
	int speed = 200;
	int score = 0;
	char road[2][16] = {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
						{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};


	while(1) 
	{
		handle_input();
		update_game(road, &car_x, &car_y, &car_jumping);
		render(road, car_x, car_y, car_jumping);
		score++;


		if (gamestate == 1)
		{
			char row_1[] = "Game Over!";
			char row_2[13] = "score: ";
			char str[6];
			snprintf(str, 6, "%05d", score);
			strcat(row_2, str);
			print_rows(row_1, row_2, LEN(row_1), LEN(row_2));
			score = 0;
			_delay_ms(2000);
			break;
		}
		_delay_ms(speed);

	}
}

void scores()
{
	PORTA |= (1 << PA6);
}

void render(char road_table[2][16], int car_x, int car_y, int car_jumping)
{
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

void update_game(char road_table[2][16], int *car_x, int *car_y, int *car_jumping)
{

	if (button_0 == 1)
	{
		*car_y = 0;
	}
	else if (button_4 == 1)
	{
		*car_y = 1;
	}

	if (button_2 == 1 && *car_jumping == -2)
	{
		*car_jumping = 3;
	}


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


	//clear first tiles
	road_table[0][0] = ' ';
	road_table[1][0] = ' ';


	//generate new obstacles
	int gates = rand() % 7;
	if (road_table[0][1] == ' ' && road_table[1][1] == ' ' && 
		 road_table[0][2] == ' ' && road_table[1][2] == ' ')
	{
		if ((gates) == 0)
		{	
			if(!(road_table[0][3] == ']' && road_table[1][3] == '['))
			{	
				road_table[0][0] = ']';
				road_table[1][0] = ']';
			}
		} 
		else if ((gates) == 1)
		{
			road_table[0][0] = ']';
		}
		else if ((gates) == 2)
		{
			road_table[1][0] = ']';
		}
	}


		// check for death
	if (road_table[*car_y][*car_x] == ']' && *car_jumping <= 0)
	{
		gamestate = 1;
	}

	if (*car_jumping > -2) 
	{
		*car_jumping += -1;  
	}
		
	
	
}
