/*
 * Keypad2Nokia5110.c
 *
 * Created: 4/13/2018 1:10:49 AM
 * Author : Paul
 */ 

#include <avr/io.h>
#include <string.h>
#include "keypad.h"
#include "nokia5110.h"


int main(void)
{
	/*
	when scaling by 1: 14 chars per line, 8 pixels in height
	when scaling by 2: 8 chars per line, 14 pixels in height
	when scaling by 3: 5 chars per line, 21 pixels in height
	*/
	DDRC = 0x0F;	//Set PC7...PC3 to input, PC2...PC0 to output [0000 1111]
	PORTC = 0xF0;	//Init port C to 1s							[1111 0000]
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 1s
	nokia_lcd_init();
	nokia_lcd_clear();
	
	char intro[] = "Hello";
	nokia_lcd_write_string(intro,1);
	nokia_lcd_render();
	unsigned char key = '\0';
	unsigned char tmp = '\0';
    while (1) 
    {
		tmp = GetKeypadKey();
		if(key != tmp){
			nokia_lcd_clear();
			nokia_lcd_write_char(key,1);
			unsigned char test = 141;
			strcpy(intro, test);
			nokia_lcd_write_string(intro,1);
			nokia_lcd_render();
			key = tmp;
		}
    }
}

