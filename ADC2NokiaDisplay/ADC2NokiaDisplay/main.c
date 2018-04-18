/*
 * ADC2NokiaDisplay.c
 *
 * Created: 4/17/2018 3:54:00 PM
 * Author : Paul
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "nokia5110.h"
#include "adc.h"
#include "bit.h"
#include "HX711.h"



int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    //adc_init();
    nokia_lcd_init();
	HX711_init();
    HX711_powerUp();
	HX711_setGain(32);
    int32_t joystick_value = HX711_read();
	HX711_powerDown();
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
	
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_char(hundred_thousands + '0', 1);
	nokia_lcd_set_cursor(6,10);
	nokia_lcd_write_char(ten_thousands + '0', 1);
	nokia_lcd_set_cursor(12,10);
	nokia_lcd_write_char(thousands + '0', 1);
	
    nokia_lcd_set_cursor(0,20);
    nokia_lcd_write_char(hundreds + '0', 1);
    nokia_lcd_set_cursor(6,20);
    nokia_lcd_write_char(tens + '0', 1);
    nokia_lcd_set_cursor(12,20);
    nokia_lcd_write_char(ones + '0', 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("Old Val", 1);
    nokia_lcd_render();
	
	_delay_ms(200); 
    
    while (1)
    {
		HX711_powerUp();
		HX711_setGain(32);
	    new_joy_value = HX711_read();
		HX711_powerDown();
	    
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
			nokia_lcd_write_string("failed to",1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string("read data :(",1);
			nokia_lcd_render();
			
			/*
			nokia_lcd_clear();
		    nokia_lcd_write_char(ten_millions + '0', 1);
		    nokia_lcd_set_cursor(6,0);
		    nokia_lcd_write_char(millions + '0', 1);
		    
		    nokia_lcd_set_cursor(0,10);
		    nokia_lcd_write_char(hundred_thousands + '0', 1);
		    nokia_lcd_set_cursor(6,10);
		    nokia_lcd_write_char(ten_thousands + '0', 1);
		    nokia_lcd_set_cursor(12,10);
		    nokia_lcd_write_char(thousands + '0', 1);
		    
		    nokia_lcd_set_cursor(0,20);
		    nokia_lcd_write_char(hundreds + '0', 1);
		    nokia_lcd_set_cursor(6,20);
		    nokia_lcd_write_char(tens + '0', 1);
		    nokia_lcd_set_cursor(12,20);
		    nokia_lcd_write_char(ones + '0', 1);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string("New Val", 1);
		    nokia_lcd_render();
			*/
	    }
    }
}

