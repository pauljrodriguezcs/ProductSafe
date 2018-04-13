/*
 * Nokia5110Test.c
 *
 * Created: 4/13/2018 12:19:39 AM
 * Author : Paul
 */ 

#include <avr/io.h>
#include "nokia5110.h"



int main(void)
{
	/*
	when scaling by 1: 14 chars per line, 8 pixels in height
	when scaling by 2: 8 chars per line, 14 pixels in height
	when scaling by 3: 5 chars per line, 21 pixels in height
	
	
	
	
	
	*/
	DDRD = 0xFF;
	PORTD = 0x00;
    /* Replace with your application code */
	nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_write_string("Call me daddy",1);

	nokia_lcd_render();
    while (1) 
    {
		
    }
}

