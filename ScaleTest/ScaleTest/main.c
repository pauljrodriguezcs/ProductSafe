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
#include "HX711_1.h"

			
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
	
	weight = HX711_1_get_units(10);
	if(weight < 0){
		weight = 0;
	}
	
	
	while(weight != 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Remove items  from scale",1);
		nokia_lcd_render();
		
		weight = HX711_1_get_units(10);
		if(weight < 0){
			weight = 0;
		}
	}
	
	weight = HX711_1_get_units(10);
	if(weight < 0){
		weight = 0;
	}
	
	while(weight == 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Weigh item",1);
		nokia_lcd_render();
		
		weight = HX711_1_get_units(10);
		if(weight < 0){
			weight = 0;
		}
	}
	
	weight = HX711_1_get_units(10);
	//output_value(weight);
	compare_weight = weight;

	while(weight != 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Remove items  from scale",1);
		nokia_lcd_render();
		
		weight = HX711_1_get_units(10);
		if(weight < 0){
			weight = 0; 
		}
	}
	
	weight = HX711_1_get_units(10);
	if(weight < 0){
		weight = 0;
	}
	
	while(weight == 0){
		nokia_lcd_clear();
		nokia_lcd_write_string("Weigh item",1);
		nokia_lcd_render();
		
		weight = HX711_1_get_units(10);
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
	
	//max_weight = HX711_read_average(10);
	//min_weight = HX711_read_average(10);

	max_weight = HX711_1_get_units(1);
	min_weight = HX711_1_get_units(1);
	
	while(1){
		//weight = HX711_read();
		//weight = HX711_read_average(10);
		weight = HX711_1_get_units(10);
		
		/*
		// finds the max values
		for(unsigned char t = 0; t < 10; t++){
			//weight = HX711_read();
			//weight = HX711_read_average(10);
			weight = HX711_get_units(1);
			
			if(weight > max_weight){
				max_weight = weight;
			}
		}
		
		output_value(max_weight);
		*/
		
		/*
		// finds the min values
		for(unsigned char t = 0; t < 10; t++){
			//weight = HX711_read();
			//weight = HX711_read_average(10);
			weight = HX711_get_units(1);
			
			if(weight < min_weight){
				min_weight = weight;
			}
		}
		
		output_value(min_weight);
		*/
		
		output_value(weight);
	}
	
	return;
}

void test_scale(){
	long weight = 0;
	while(1){
		weight = HX711_1_get_units(10);
		output_value(weight);
	}
}
 
int main(void) 
{ 
	//////HX711_init(128);						// values for 1kg scale
	//////HX711_set_offset(8409384);			// values for 1kg scale
	//////HX711_set_scale(-946.4825995);		// values for 1kg scale
	
	//////HX711_init(64);						// values for 5kg scale
	//////HX711_set_offset(8404767);			// values for 5kg scale
	//////HX711_set_scale(-160.6693354);		// values for 5kg scale
	
	/*
	b_32 = [8385785,8385831] range() = 46
	b_64 = [8394727,8399057] range() = 4330
	b_128 = [8409384,8411163] range() = 1779
	
	x_128 = [7889134,7894109] range() = 4975
	
	m_max = -1.065429803
	m_avg = -1.065652991
	m_min = -1.065876179
	
	y_min = 481094 / 544 = 884.3639706
	y_max = 485251 / 544 = 892.0055147
	
	y_min = 120343 / 138 = 872.0507246
	y_max = 124741 / 138 = 903.9202899
	
	*/
	
	DDRD = 0xFF; PORTD = 0x00;
	
	nokia_lcd_init();
	
	HX711_1_init(128);
	//HX711_1_set_offset(8409384);
	//HX711_1_set_scale(-946.4825995);	
					
	calibrate();
	//test();
	//test_scale();
	
	return 0; 
}