/*
 * Nokia+Joystick.c
 *
 * Created: 4/14/2018 8:26:26 PM
 * Author : Paul
 */ 

#include <avr/io.h>
#include "nokia5110.h"
//code modified from http://maxembedded.com/2011/06/the-adc-of-the-avr/----------------------------
void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convertion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}
//end of code modified from http://maxembedded.com/2011/06/the-adc-of-the-avr/---------------------

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	adc_init();
	nokia_lcd_init();
	
	unsigned short joystick_value = adc_read(0);
	unsigned short new_joy_value;
	unsigned char thousands = 0;
	unsigned char hundreds = 0;
	unsigned char tens = 0;
	unsigned char ones = 0;
	
	nokia_lcd_clear();
	
	if(joystick_value > 1000){
		thousands = joystick_value / 1000;
		joystick_value = joystick_value % 1000;
	}
	
	if(joystick_value > 100){
		hundreds = joystick_value / 100;
		joystick_value = joystick_value % 100;
	}
	
	if(joystick_value > 10){
		tens = joystick_value / 10;
		joystick_value = joystick_value % 10;
	}
		
	nokia_lcd_write_char(thousands + '0', 1);
	nokia_lcd_set_cursor(6,10);
	nokia_lcd_write_char(hundreds + '0', 1);
	nokia_lcd_set_cursor(12,10);
	nokia_lcd_write_char(tens + '0', 1);
	nokia_lcd_set_cursor(18,10);
	nokia_lcd_write_char(ones + '0', 1);
	nokia_lcd_render();
	
    while (1) 
    {
		new_joy_value = adc_read(0);
		
		if(new_joy_value != joystick_value){
			thousands = 0;
			hundreds = 0;
			tens = 0;
			ones = 0;
			
			joystick_value = new_joy_value;
			
			if(joystick_value > 1000){
				thousands = joystick_value / 1000;
				joystick_value = joystick_value % 1000;
			}
			
			if(joystick_value > 100){
				hundreds = joystick_value / 100;
				joystick_value = joystick_value % 100;
			}
			
			if(joystick_value > 10){
				tens = joystick_value / 10;
				joystick_value = joystick_value % 10;
			}
			
			nokia_lcd_clear();
			nokia_lcd_write_char(thousands + '0', 1);
			nokia_lcd_set_cursor(6,0);
			nokia_lcd_write_char(hundreds + '0', 1);
			nokia_lcd_set_cursor(12,0);
			nokia_lcd_write_char(tens + '0', 1);
			nokia_lcd_set_cursor(18,0);
			nokia_lcd_write_char(ones + '0', 1);
			nokia_lcd_render();
		}
    }
}

