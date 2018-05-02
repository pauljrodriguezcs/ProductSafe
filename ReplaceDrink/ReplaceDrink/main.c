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

unsigned char keys_in_safe_flag = 0;	// 0 = false, 1 = true;

unsigned char replace_drink_flag = 1;
unsigned char liquor_door_signal = 1;	// 0 = unlock, 1 = lock
unsigned char liquor_door_sensor = 0;	// 0 = door open, 1 = door closed
unsigned char type_of_drink = 0;
unsigned char add_drink_flag = 0;

void replace_drink_error(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Remove keys   from safe.    Press any key to continue.",1);
	nokia_lcd_render();
}

void replace_drink_success(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Replace drink.",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("Then close thedoor to continue.",1);
	nokia_lcd_render();
}

enum ReplaceDrinkState {replace_init,replace_fail,replace_no_drink,replace_open_door,replace_add_drink} replace_drink_state;

void ReplaceDrink_Init(){
	PORTB = liquor_door_signal;
	replace_drink_state = replace_init;
}

void ReplaceDrink_Tick(){
	unsigned char replace_drink_selection = '\0';
	unsigned char replace_drink_prev_select = '\0';
	//Actions
	switch(replace_drink_state){
		case replace_init:
			break;
			
		case replace_fail:
			while((replace_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((replace_drink_prev_select = GetKeypadKey()) == replace_drink_selection){ _delay_ms(200); }
			break;
		
		case replace_no_drink:
			while((replace_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((replace_drink_prev_select = GetKeypadKey()) == replace_drink_selection){ _delay_ms(200); }
			break;
			
		case replace_open_door:
			liquor_door_sensor = PINA & 0x01;
			break;
			
		case replace_add_drink:
			break;
			
		default:
			break;
	}
	//Transitions
	switch(replace_drink_state){
		case replace_init:
			if(replace_drink_flag){
				if(keys_in_safe_flag){
					replace_drink_error();
					replace_drink_state = replace_fail;
				}
				
				else if(type_of_drink == 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("No drink      detected.     Press any key to continue.",1);
					nokia_lcd_render();
					replace_drink_state = replace_no_drink;
				}
				
				else{
					liquor_door_signal = 0;
					PORTB = liquor_door_signal;
					replace_drink_success();
					replace_drink_state = replace_open_door;
				}
			}
			break;
		
		case replace_fail:
			if(replace_drink_selection != '\0'){
				replace_drink_flag = 0;
				replace_drink_state = replace_init;
			}
			break;
			
		case replace_no_drink:
			if(replace_drink_selection != '\0'){
				replace_drink_flag = 0;
				replace_drink_state = replace_init;
			}
			break;
		
		case replace_open_door:
			if(liquor_door_sensor == 1){
				liquor_door_signal = 1;
				PORTB = liquor_door_signal;
				add_drink_flag = 1;
				type_of_drink = 0;
				nokia_lcd_clear();
				nokia_lcd_write_string("*add drink*",1);
				nokia_lcd_render();
				replace_drink_state = replace_add_drink;
			}
			
			else{
				replace_drink_state = replace_open_door;
			}
			break;
		
		
		case replace_add_drink:
			if(add_drink_flag){
				replace_drink_state = replace_add_drink;
			}
			
			else{				
				replace_drink_state = replace_init;
			}
			break;
		
		default:
			replace_drink_state = replace_init;
			break;
	}
}

void ReplaceDrinkTask()
{
	ReplaceDrink_Init();
	for(;;) 
	{ 	
		ReplaceDrink_Tick();
	}	vTaskDelay(100); 
	 
}

void ReplaceDrinkPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(ReplaceDrinkTask, (signed portCHAR *)"ReplaceDrinkTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
	DDRC = 0x0F;	//Set PC7...PC3 to input, PC2...PC0 to output [0000 1111]
	PORTC = 0xF0;	//Init port C to 1s							[1111 0000]
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 0s
	DDRB = 0xFF;	//Controls the locks
	PORTB = 0x00;
	DDRA = 0x00;	//Controls the sensors to make sure door is locked
	PORTA = 0xFF;	
	nokia_lcd_init();
	//Start Tasks  
	ReplaceDrinkPulse(1);
    //RunSchedular 
	vTaskStartScheduler(); 
 
	return 0; 
}