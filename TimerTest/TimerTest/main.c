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
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

#include "nokia5110.h"

unsigned char drink_has_been_removed = 1; 
uint16_t time_elapsed_counter = 0;

unsigned char timer_is_on = 1;
uint16_t time_remaining_counter = 61520;

enum display {disp} display_state;
	
void display_Init(){
	display_state = disp;
}

void display_Tick(){
	//Actions
	switch(display_state){
		case disp:
			;
			uint16_t temp_time = time_elapsed_counter / 2 ;
			unsigned char hours = temp_time / 3600;
			unsigned char minutes = (temp_time % 3600) / 60;
			unsigned char seconds = temp_time % 60; 
			unsigned char hours_tens = hours / 10;
			unsigned char hours_ones = hours % 10;
			unsigned char minutes_tens = minutes / 10;
			unsigned char minutes_ones = minutes % 10;
			unsigned char seconds_tens = seconds / 10;
			unsigned char seconds_ones = seconds % 10;
			
			nokia_lcd_clear();
			nokia_lcd_write_string("Elapsed Time",1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_char(hours_tens + '0',1);
			nokia_lcd_write_char(hours_ones + '0',1);
			nokia_lcd_write_char(':',1);
			nokia_lcd_write_char(minutes_tens + '0',1);
			nokia_lcd_write_char(minutes_ones + '0',1);
			nokia_lcd_write_char(':',1);
			nokia_lcd_write_char(seconds_tens + '0',1);
			nokia_lcd_write_char(seconds_ones + '0',1);
			nokia_lcd_render();
			break;
		

		default:
		break;
	}
	
	//Transitions
	switch(display_state){
		case disp:
		break;
		
		default:
		display_state = disp;
		break;
	}
}

void displayTask(){
	display_Init();
	for(;;){
		display_Tick();
		vTaskDelay(100);
	}
}

void displayPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(displayTask, (signed portCHAR *)"displayTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}


/////// Time-elapsed since drink has been removed state machine /////
enum TimeElapsedState {te_wait, te_counter} time_elapsed_state;

void TimeElapsed_Init(){
	time_elapsed_state = te_wait;
}

void TimeElapsed_Tick(){
	//Actions
	switch(time_elapsed_state){
		case te_wait:
			break;
		
		case te_counter:
			++time_elapsed_counter;
			if((~PINA) & 0x01){
				drink_has_been_removed = 0;
			}
			break;
		
		default:
			break;
	}
	
	//Transitions
	switch(time_elapsed_state){
		case te_wait:
			if(drink_has_been_removed){
				time_elapsed_counter = 35997;
				time_elapsed_state = te_counter;
			}
		
			else{
				time_elapsed_state = te_wait;
			}
		
			break;
		
		case te_counter:
			if(!drink_has_been_removed){
				time_elapsed_state = te_wait;
			}
		
			else{
				time_elapsed_state = te_counter;
			}
			break;
		
		default:
			time_elapsed_state = te_wait;
			break;
	}
}

void TimeElapsedTask(){
	TimeElapsed_Init();
	for(;;){
		TimeElapsed_Tick();
		vTaskDelay(500);
	}
}

void TimeElapsedPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(TimeElapsedTask, (signed portCHAR *)"TimeElapsedTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

/////// Time-elapsed since drink has been removed state machine /////

enum TimeRemainingState {trs_wait, trs_counter} time_remaining_state;

void TimeRemaining_Init(){
	time_remaining_state = trs_wait;
}

void TimeRemaining_Tick(){
	//Actions
	switch(time_remaining_state){
		case trs_wait:
		break;
		
		case trs_counter:
		if(time_remaining_counter > 0){
			--time_remaining_counter;
		}
		break;
		
		default:
		break;
	}
	
	//Transitions
	switch(time_remaining_state){
		case trs_wait:
		if(timer_is_on){
			time_remaining_state = trs_counter;
		}
		
		break;
		
		case trs_counter:
		if(time_remaining_counter == 0){
			timer_is_on = 0;
			time_remaining_state = trs_wait;
		}
		break;
		
		default:
		time_remaining_state = trs_wait;
		break;
	}
}

void TimeRemainingTask(){
	TimeRemaining_Init();
	for(;;){
		TimeRemaining_Tick();
		vTaskDelay(500);
	}
}

void TimeRemainingPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(TimeRemainingTask, (signed portCHAR *)"TimeRemainingTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}


int main(void) 
{ 
	DDRA = 0x00;	//Controls the sensors to make sure door is locked
	PORTA = 0xFF;
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 0s
   
	nokia_lcd_init();
   
	//Start Tasks  
	displayPulse(1);
	TimeElapsedPulse(1);
	//TimeRemainingPulse(1);
   
    //RunSchedular 
	vTaskStartScheduler(); 
 
	return 0; 
}