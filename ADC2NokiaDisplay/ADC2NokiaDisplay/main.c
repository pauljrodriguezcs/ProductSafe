/*
 * ADC2NokiaDisplay.c
 *
 * Created: 4/17/2018 3:54:00 PM
 * Author : Paul
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include "nokia5110.h"
#include "bit.h"

// HX711 helper functions

unsigned char GAIN;

void HX711_setGain(unsigned char gain){
	switch(gain){
		case 128:
			GAIN = 1;
			break;
			
		case 32:
			GAIN = 2;
			break;
			
		case 64:
			GAIN = 3;
			break;
			
		default:
			GAIN = 1;
			break;
	}
	
	PORTB = 0x00; _NOP(); _NOP(); _NOP(); _NOP();
}

unsigned char HX711_isReady(){
	if(PINA & 0x01){
		return 1;
	}
	
	else{
		return 0;
	}
}

int32_t HX711_read(){
	int32_t value = 0;
		
	while(!HX711_isReady()){ }
	
	for(unsigned char i = 0; i < 24; i++){
		PORTB = 0xFF; _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
		value = value << 1;
		PORTB = 0x00; _NOP(); _NOP(); _NOP(); _NOP();
		
		if(GetBit(PINA,0)){
			value++;
		}
		
	}
	
	for(unsigned char i = 0; i < GAIN; i++){
		PORTB = 0xFF; _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
		PORTB = 0x00; _NOP(); _NOP(); _NOP(); _NOP();
	}
	
	value = value ^ 0x80000;
	
	return value;
}


int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    //adc_init();
    nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_write_string("Welcome",1);
	nokia_lcd_render();
	
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("Reading...",1);
	nokia_lcd_render();
	
	PORTB = 0x00;
	PORTB = 0xFF;
	
	_delay_us(70);
	
	
	HX711_setGain(64);
    int32_t joystick_value = HX711_read();
	
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string("Done...",1);
	nokia_lcd_render();
	
    int32_t new_joy_value;
	
	unsigned char ten_millions = 0;
	unsigned char millions = 0;
	unsigned char hundred_thousands = 0;
	unsigned char ten_thousands = 0;
    unsigned char thousands = 0;
    unsigned char hundreds = 0;
    unsigned char tens = 0;
    unsigned char ones = 0;
    
    nokia_lcd_clear();
    
	if(joystick_value > 10000000){
		ten_millions = joystick_value / 10000000;
		joystick_value = joystick_value % 10000000;
	}
	
	if(joystick_value > 1000000){
		millions = joystick_value / 1000000;
		joystick_value = joystick_value % 1000000;
	}
	
	if(joystick_value > 100000){
		hundred_thousands = joystick_value / 100000;
		joystick_value = joystick_value % 100000;
	}
	
	if(joystick_value > 10000){
		ten_thousands = joystick_value / 10000;
		joystick_value = joystick_value % 10000;
	}
	
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
    
	nokia_lcd_write_char(ten_millions + '0', 1);
	nokia_lcd_set_cursor(6,0);
	nokia_lcd_write_char(millions + '0', 1);
	
	nokia_lcd_set_cursor(14,0);
	nokia_lcd_write_char(hundred_thousands + '0', 1);
	nokia_lcd_set_cursor(20,0);
	nokia_lcd_write_char(ten_thousands + '0', 1);
	nokia_lcd_set_cursor(26,0);
	nokia_lcd_write_char(thousands + '0', 1);
	
    nokia_lcd_set_cursor(34,0);
    nokia_lcd_write_char(hundreds + '0', 1);
    nokia_lcd_set_cursor(40,0);
    nokia_lcd_write_char(tens + '0', 1);
    nokia_lcd_set_cursor(46,0);
    nokia_lcd_write_char(ones + '0', 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("Old Val", 1);
	
    nokia_lcd_render();
	
	_delay_ms(200);
	
    while (1)
    {
		PORTB = 0x00;
		PORTB = 0xFF;
		
		_delay_us(70);
		
		PORTB = 0x00;
		
	    new_joy_value = HX711_read();
	    
	    if(new_joy_value != joystick_value){
			ten_millions = 0;
			millions = 0;
			hundred_thousands = 0;
			ten_thousands = 0;
		    thousands = 0;
		    hundreds = 0;
		    tens = 0;
		    ones = 0;
		    
		    joystick_value = new_joy_value;
		    
		    if(joystick_value > 10000000){
			    ten_millions = joystick_value / 10000000;
			    joystick_value = joystick_value % 10000000;
		    }
		    
		    if(joystick_value > 1000000){
			    millions = joystick_value / 1000000;
			    joystick_value = joystick_value % 1000000;
		    }
		    
		    if(joystick_value > 100000){
			    hundred_thousands = joystick_value / 100000;
			    joystick_value = joystick_value % 100000;
		    }
		    
		    if(joystick_value > 10000){
			    ten_thousands = joystick_value / 10000;
			    joystick_value = joystick_value % 10000;
		    }
		    
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
		    nokia_lcd_write_char(ten_millions + '0', 1);
		    nokia_lcd_set_cursor(6,0);
		    nokia_lcd_write_char(millions + '0', 1);
		    
		    nokia_lcd_set_cursor(14,0);
		    nokia_lcd_write_char(hundred_thousands + '0', 1);
		    nokia_lcd_set_cursor(20,0);
		    nokia_lcd_write_char(ten_thousands + '0', 1);
		    nokia_lcd_set_cursor(26,0);
		    nokia_lcd_write_char(thousands + '0', 1);
		    
		    nokia_lcd_set_cursor(34,0);
		    nokia_lcd_write_char(hundreds + '0', 1);
		    nokia_lcd_set_cursor(40,0);
		    nokia_lcd_write_char(tens + '0', 1);
		    nokia_lcd_set_cursor(46,0);
		    nokia_lcd_write_char(ones + '0', 1);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string("New Val", 1);
		    nokia_lcd_render();
		
	    }
		
		nokia_lcd_set_cursor(0, 30);
		nokia_lcd_write_string("Old Val", 1);
		nokia_lcd_render();
    }
}

