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
	
	unsigned char old = adc_read(0);
	
	if(old > 540){
		PORTB = 0xFF;
	}
	
	else{
		PORTB = 0x00;
	}
	unsigned char new;
	
	nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_write_string("Hello",1);
	nokia_lcd_render();
	
    /* Replace with your application code */
    while (1) 
    {
		new = adc_read(0);
		
		
		if(new > 540){
			PORTB = 0xFF;
		}
		
		else{
			PORTB = 0x00;
		}
    }
}

