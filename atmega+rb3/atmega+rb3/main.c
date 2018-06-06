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
#include "usart_ATmega1284.h"

unsigned char data = 0;

enum DataRecieveState {read_wait, read_collect, read_display} data_recieve_state;

void DataRecieve_Init(){
	data_recieve_state = read_wait;
} 

void DataRecieve_Tick(){
	//Actions
	switch (data_recieve_state){
		case read_wait:
			break;
		
		case read_collect:
			data = USART_Receive(0);
			break;
			
		case read_display:
			;
			unsigned char hundreds = data / 100;
			unsigned char tens = (data % 100) / 10;
			unsigned char ones = (data % 100) % 10;
			nokia_lcd_clear();
			nokia_lcd_write_char(hundreds + '0',1);
			nokia_lcd_write_char(tens + '0',1);
			nokia_lcd_write_char(ones + '0',1);
			nokia_lcd_render();
			break;
			
		default:
			break;
	}
	
	//Transitions
	switch (data_recieve_state){
		case read_wait:
			if(USART_HasReceived(0)){
				data_recieve_state = read_collect;
			}
			break;
		
		case read_collect:
			data_recieve_state = read_display;
			break;
		
		case read_display:
			data_recieve_state = read_wait;
			break;
		
		default:
			data_recieve_state = read_wait;
			break;
	}
}

void DataRecieve_Task(){
	DataRecieve_Init();
	for(;;){
		DataRecieve_Tick();
		vTaskDelay(100);
	}
	
}

void DataRecievePulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(DataRecieve_Task, (signed portCHAR *)"DataRecieve_Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
 
int main(void) 
{ 
	DDRC = 0xFF;
	PORTC = 0x00;
	nokia_lcd_init();
	initUSART(0);
	nokia_lcd_clear();
	nokia_lcd_write_string("Waiting",1);
	nokia_lcd_render();
	
	//Start Tasks	
	DataRecievePulse(1);
    //RunSchedular 
	vTaskStartScheduler(); 
 
	return 0; 
}