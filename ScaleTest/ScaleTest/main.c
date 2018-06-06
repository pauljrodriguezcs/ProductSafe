#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 
#include <util/delay.h>
 


#include "nokia5110.h"
#include "HX711.h"

			
void output_value(long data_value){
	
	if(data_value < 0){
		data_value = 0;
	}
	
	
	unsigned char billion;
	unsigned char h_million;
	unsigned char t_million;
	unsigned char o_million;
	unsigned char h_thousand;
	unsigned char t_thousand;
	unsigned char o_thousand;
	unsigned char hundred;
	unsigned char ten;
	unsigned char one;
			
	billion = data_value / 1000000000;
	data_value = data_value % 1000000000;
			
	h_million = data_value / 100000000;
	data_value = data_value % 100000000;
			
	t_million = data_value / 10000000;
	data_value = data_value % 10000000;
			
	o_million = data_value / 1000000;
	data_value = data_value % 1000000;
			
	h_thousand = data_value / 100000;
	data_value = data_value % 100000;
			
	t_thousand = data_value / 10000;
	data_value = data_value % 10000;
			
	o_thousand = data_value / 1000;
	data_value = data_value % 1000;
			
	hundred = data_value / 100;
	data_value = data_value % 100;
			
	ten = data_value / 10;
			
	one = data_value % 10;
			
	nokia_lcd_clear();
	nokia_lcd_write_char(billion + '0',1);
	nokia_lcd_write_char(h_million + '0',1);
	nokia_lcd_write_char(t_million + '0',1);
	nokia_lcd_write_char(o_million + '0',1);
	nokia_lcd_write_char(h_thousand + '0',1);
	nokia_lcd_write_char(t_thousand + '0',1);
	nokia_lcd_write_char(o_thousand + '0',1);
	nokia_lcd_write_char(hundred + '0',1);
	nokia_lcd_write_char(ten + '0',1);
	nokia_lcd_write_char(one + '0',1);
	nokia_lcd_render();
	
}

void test(){
	long weight = 0;
	long compare_weight = 0;
	
	weight = HX711_get_units(10);
	if(weight < 0){
		weight = 0;
	}
	
	
	while(weight != 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Remove items  from scale",1);
		nokia_lcd_render();
		
		weight = HX711_get_units(10);
		if(weight < 0){
			weight = 0;
		}
	}
	
	weight = HX711_get_units(10);
	if(weight < 0){
		weight = 0;
	}
	
	while(weight == 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Weigh item",1);
		nokia_lcd_render();
		
		weight = HX711_get_units(10);
		if(weight < 0){
			weight = 0;
		}
	}
	
	weight = HX711_get_units(10);
	//output_value(weight);
	compare_weight = weight;

	while(weight != 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Remove items  from scale",1);
		nokia_lcd_render();
		
		weight = HX711_get_units(10);
		if(weight < 0){
			weight = 0; 
		}
	}
	
	weight = HX711_get_units(10);
	if(weight < 0){
		weight = 0;
	}
	
	while(weight == 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Weigh item",1);
		nokia_lcd_render();
		
		weight = HX711_get_units(10);
		if(weight < 0){
			weight = 0;
		}
	}
	
	if((compare_weight - 1 < weight) || (weight < compare_weight + 1)){
		nokia_lcd_clear();
		nokia_lcd_write_string("Item within +/- 1 tolerance",1);
		nokia_lcd_render();
	}
	
	else{
		nokia_lcd_clear();
		nokia_lcd_write_string("Item not within +/- 1 tolerance",1);
		nokia_lcd_render();
	}
	
	return;
}

void calibrate(){
	long weight = 0;
	long max_weight = 0;
	long min_weight = 0;
	
	max_weight = HX711_read_average(10);
	min_weight = HX711_read_average(10);
	//max_weight = HX711_get_units(10);
	//min_weight = HX711_get_units(10);
	
	while(1){
		//weight = HX711_read();
		weight = HX711_read_average(10);
		//weight = HX711_get_units(10);
		
		///*
		// finds the max values
		for(unsigned char t = 0; t < 10; t++){
			
			weight = HX711_get_units(10);
			
			if(weight > max_weight){
				max_weight = weight;
			}
		}
		
		output_value(max_weight);
		//*/
		
		/*
		// finds the min values
		for(unsigned char t = 0; t < 10; t++){
			
			weight = HX711_read_average(10);
			//weight = HX711_get_units(10);
			
			if(weight < min_weight){
				min_weight = weight;
			}
		}
		
		output_value(min_weight);
		*/
		
		//output_value(weight);
	}
	
	return;
}
 
int main(void) 
{ 
	DDRA = 0x00; PORTA=0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	nokia_lcd_init();
	
	nokia_lcd_clear();
	nokia_lcd_write_string("pre-init",1);
	nokia_lcd_render();
	
	HX711_init(32);
	
	nokia_lcd_clear();
	nokia_lcd_write_string("post-init",1);
	nokia_lcd_render();
		
	//////HX711_init(64);						// values for 5kg scale
	//////HX711_set_offset(8404767);			// values for 5kg scale
	//////HX711_set_scale(-160.6693354);		// values for 5kg scale
	
	
	
	/*
	b_32 = [ , ] range() = 
	b_64 = [8394727,8399057] range() = 4330
	b_128 = [ , ] range() = 
	
	
	
	
	
	*/
						
	calibrate();
	//test();
	
	return 0; 
}