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

 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

#include "keypad.h"
#include "nokia5110.h"

unsigned char add_drink_flag = 1;
unsigned char add_drink_selection = '\0';
unsigned char add_drink_prev_select = '\0';
unsigned char type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
uint16_t volume_of_drink = 0;
unsigned char drink_alcohol_content = 0;
char *drink_names[6] = {"*", "Beer", "Wine", "Liquor", "Other"};
char number_output[5] = "_   ";

void add_drink_display(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Type of drink",1);
	nokia_lcd_set_cursor(2,10);
	nokia_lcd_write_string("1: Beer",1);
	nokia_lcd_set_cursor(2,20);
	nokia_lcd_write_string("2: Wine",1);
	nokia_lcd_set_cursor(2,30);
	nokia_lcd_write_string("3: Liquor",1);
	nokia_lcd_set_cursor(2,40);
	nokia_lcd_write_string("4: Other",1);
	nokia_lcd_render();
}

void add_drink_volume_display(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Enter",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("Volume (mL)",1);
	nokia_lcd_render();
}

void add_drink_ac_display(){	//ac = alcohol content
	nokia_lcd_clear();
	nokia_lcd_write_string("Enter Alcohol",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("Content %",1);
	nokia_lcd_render();
}

void add_drink_variable_reset(){
	add_drink_selection = '\0';
	add_drink_prev_select = '\0';
	number_output[0] = '_';
	number_output[1] = '\0';
	number_output[2] = '\0';
	number_output[3] = '\0';
	
}

enum AddDrinkState {add_drink_init,drink_exists,drink_type,drink_volume,drink_ac,drink_confirm} add_drink_state;	
	
void AddDrink_Init(){
	add_drink_state = add_drink_init;
}

void AddDrink_Tick(){
	//Actions
	switch(add_drink_state){
		case add_drink_init:
			break;
			
		case drink_exists:
			while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
			break;
			
		case drink_type:
			while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
			break;
			
		case drink_volume:
			while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
				
			while(add_drink_selection != '#'){
				if(add_drink_selection != '\0' && add_drink_selection != '*' && volume_of_drink == 0){
					volume_of_drink = add_drink_selection - '0';
					number_output[0] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Volume (mL)",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" (mL)",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection != '\0' && add_drink_selection != '*' && volume_of_drink >= 1 && volume_of_drink <= 9){
					volume_of_drink = (volume_of_drink * 10) + (add_drink_selection - '0');
					number_output[1] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Volume (mL)",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" (mL)",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection != '\0' && add_drink_selection != '*' && volume_of_drink >= 10 && volume_of_drink <= 99){
					volume_of_drink = (volume_of_drink * 10) + (add_drink_selection - '0');
					number_output[2] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Volume (mL)",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" (mL)",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection != '\0' && add_drink_selection != '*' && volume_of_drink >= 100 && volume_of_drink <= 999){
					volume_of_drink = (volume_of_drink * 10) + (add_drink_selection - '0');
					number_output[3] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Volume (mL)",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" (mL)",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection == '*'){
					volume_of_drink = 0;
					number_output[0] = '_';
					number_output[1] = '\0';
					number_output[2] = '\0';
					number_output[3] = '\0';
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Volume (mL)",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" (mL)",1);
					nokia_lcd_render();
				}
				
				while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
			}
			break;
			
		case drink_ac:
			while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
			
			while(add_drink_selection != '#'){
				if(add_drink_selection != '\0' && add_drink_selection != '*' && drink_alcohol_content == 0){
					drink_alcohol_content = add_drink_selection - '0';
					number_output[0] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter Alcohol",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Content %",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" %",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection != '\0' && add_drink_selection != '*' && drink_alcohol_content >= 1 && drink_alcohol_content <= 9){
					drink_alcohol_content = (drink_alcohol_content * 10) + (add_drink_selection - '0');
					number_output[1] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter Alcohol",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Content %",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" %",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection != '\0' && add_drink_selection != '*' && drink_alcohol_content >= 10 && drink_alcohol_content <= 99){
					drink_alcohol_content = (drink_alcohol_content * 10) + (add_drink_selection - '0');
					number_output[2] =  add_drink_selection;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter Alcohol",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Content %",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" %",1);
					nokia_lcd_render();
				}
				
				else if(add_drink_selection == '*'){
					drink_alcohol_content = 0;
					number_output[0] = '_';
					number_output[1] = '\0';
					number_output[2] = '\0';
					number_output[3] = '\0';
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter Alcohol",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Content %",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(number_output,1);
					nokia_lcd_write_string(" %",1);
					nokia_lcd_render();
				}
			
				
				while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
				
				}
			break;
		
		case drink_confirm:
			while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
			break;

		default:
			break;
	}
	
	//Transitions
	switch(add_drink_state){
		case add_drink_init:
			if(add_drink_flag){
				if(type_of_drink != 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("Can't add",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("anymore drinks",1);
					nokia_lcd_render();
					add_drink_state = drink_exists;
				}
				
				else{
					add_drink_display();
					add_drink_variable_reset();
					add_drink_state = drink_type;
				}
			}
			break;
		
		case drink_exists:
			if(add_drink_selection == '#' && add_drink_flag){
				add_drink_flag = 0;
				add_drink_variable_reset();
				add_drink_state = add_drink_init;
			}
			
			else if(!add_drink_flag){
				type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
				volume_of_drink = 0;
				drink_alcohol_content = 0;
				add_drink_variable_reset();
				add_drink_flag = 0;
				add_drink_state = add_drink_init;
			}
			
			else{
				add_drink_state = drink_exists;
			}
			
			break;
		
		case drink_type:
			if(add_drink_selection == '1' && add_drink_flag){
				type_of_drink = 1;
				add_drink_volume_display();
				add_drink_state = drink_volume;
			}
			
			else if(add_drink_selection == '2' && add_drink_flag){
				type_of_drink = 2;
				add_drink_volume_display();
				add_drink_state = drink_volume;
			}
			
			else if(add_drink_selection == '3' && add_drink_flag){
				type_of_drink = 3;
				add_drink_volume_display();
				add_drink_state = drink_volume;
			}
			
			else if(add_drink_selection == '4'&& add_drink_flag){
				type_of_drink = 4;
				add_drink_volume_display();
				add_drink_state = drink_volume;
			}
			
			else if(!add_drink_flag){
				type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
				volume_of_drink = 0;
				drink_alcohol_content = 0;
				add_drink_variable_reset();
				add_drink_state = add_drink_init;
			}
			
			else{
				add_drink_state = drink_type;
			}
			
			break;
		
		case drink_volume:
			if(add_drink_selection == '#' && volume_of_drink != 0 && add_drink_flag){
				number_output[0] = '_';
				number_output[1] = '\0';
				number_output[2] = '\0';
				number_output[3] = '\0';
				add_drink_ac_display();
				add_drink_state = drink_ac;
			}
			
			else if(!add_drink_flag){
				type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
				volume_of_drink = 0;
				drink_alcohol_content = 0;
				add_drink_variable_reset();
				add_drink_state = add_drink_init;
			}
			
			else{
				add_drink_state = drink_volume;
			}
			break;
		
		case drink_ac:
			if(add_drink_selection == '#' && drink_alcohol_content != 0 && add_drink_flag){
				
				nokia_lcd_clear();
				nokia_lcd_write_string(drink_names[type_of_drink],1);
				nokia_lcd_set_cursor(0,10);
				
				if(volume_of_drink > 999){
					unsigned char thousands = volume_of_drink / 1000;
					unsigned char hundreds = (volume_of_drink % 1000) / 100;
					unsigned char tens = (volume_of_drink % 1000) / (volume_of_drink % 100) / 10;
					unsigned char ones = (volume_of_drink % 1000) / (volume_of_drink % 100) / (volume_of_drink % 10);
					nokia_lcd_write_char(thousands + '0',1);
					nokia_lcd_write_char(hundreds + '0',1);
					nokia_lcd_write_char(tens + '0',1);
					nokia_lcd_write_char(ones + '0',1);
					nokia_lcd_write_string(" mL",1);
				}
				
				else{
					unsigned char hundreds = volume_of_drink / 100;
					unsigned char tens = (volume_of_drink % 100) / 10;
					unsigned char ones = (volume_of_drink % 100) / (volume_of_drink % 10);
					nokia_lcd_write_char(hundreds + '0',1);
					nokia_lcd_write_char(tens + '0',1);
					nokia_lcd_write_char(ones + '0',1);
					nokia_lcd_write_string(" mL",1);
				}
				
				nokia_lcd_set_cursor(0,20);
				
				if(drink_alcohol_content > 99){
					unsigned char hundreds = drink_alcohol_content / 100;
					unsigned char tens = (drink_alcohol_content % 100) / 10;
					unsigned char ones = (drink_alcohol_content % 100) / (drink_alcohol_content % 10);
					nokia_lcd_write_char(hundreds + '0',1);
					nokia_lcd_write_char(tens + '0',1);
					nokia_lcd_write_char(ones + '0',1);
					nokia_lcd_write_string(" %",1);
				}
				
				else{
					unsigned char tens = drink_alcohol_content / 10;
					unsigned char ones = drink_alcohol_content % 10;
					nokia_lcd_write_char(tens + '0',1);
					nokia_lcd_write_char(ones + '0',1);
					nokia_lcd_write_string(" %",1);
				}
				
				nokia_lcd_set_cursor(0,30);
				nokia_lcd_write_string("* to Cancel",1);
				nokia_lcd_set_cursor(0,40);
				nokia_lcd_write_string("# to Confirm",1);
				nokia_lcd_render();
				
				add_drink_state = drink_confirm;
			}
			
			else if(!add_drink_flag){
				type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
				volume_of_drink = 0;
				drink_alcohol_content = 0;
				add_drink_variable_reset();
				add_drink_state = add_drink_init;
			}
			
			else{
				add_drink_state = drink_ac;
			}
			break;
		
		case drink_confirm:
			if(add_drink_selection == '*'){
				type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
				volume_of_drink = 0;
				drink_alcohol_content = 0;
				add_drink_variable_reset();
				add_drink_flag = 0;
				add_drink_state = add_drink_init;
			} 
			
			else if(add_drink_selection == '#'){
				add_drink_variable_reset();
				add_drink_flag = 0;
				add_drink_state = add_drink_init;
			}
			
			else{
				add_drink_selection = drink_confirm;
			}
			
			break;
		
		default:
			add_drink_state = add_drink_init;
			break;
	}
}

void AddDrinkTask()
{
	AddDrink_Init();
	for(;;) 
	{ 	
		AddDrink_Tick();
		vTaskDelay(100); 
	} 
}

void AddDrinkPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(AddDrinkTask, (signed portCHAR *)"AddDrinkTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
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
	PORTD = 0x00;	//Init Port D to 0s
	nokia_lcd_init();
	//Start Tasks  
	AddDrinkPulse(1);
    //RunSchedular 
	vTaskStartScheduler(); 
 
	return 0; 
}