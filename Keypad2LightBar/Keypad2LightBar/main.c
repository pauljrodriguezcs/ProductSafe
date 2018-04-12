/*
 * Keypad2LightBar.c
 *
 * Created: 4/12/2018 10:51:11 AM
 * Author : Paul
 */ 

#include <avr/io.h>
#include "keypad.h"


int main(void)
{
	DDRD = 0x0F; // Set PD7...PD3 to input, PD2...PD0 to output [0000 1111]
	PORTD = 0xF0; //Init port D to 1s							[1111 0000]
	DDRC = 0xFF; // Set port C to output
	PORTC = 0x00; // Init port C to 0s
    /* Replace with your application code */
    while (1)
    {	
		char x = GetKeypadKey();
		switch (x) {
			case '\0': PORTC = 0x00; break; // All 5 LEDs on
			case '1': PORTC = 0x11; break; // hex equivalent
			case '2': PORTC = 0x12; break;
			case '3': PORTC = 0x14; break;
			case '4': PORTC = 0x21; break;
			case '5': PORTC = 0x22; break;
			case '6': PORTC = 0x24; break;
			case '7': PORTC = 0x41; break;
			case '8': PORTC = 0x42; break;
			case '9': PORTC = 0x44; break;
			case '*': PORTC = 0x81; break;
			case '0': PORTC = 0x82; break;
			case '#': PORTC = 0x84; break;
			default: PORTC = 0x00; break; // Should never occur. Middle LED off.
		}
		
    }
}

