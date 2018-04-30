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

#include "nokia5110.h"
#include "keypad.h"

////////// Start of helper function that returns Numeric & AlphaNumeric Values //////////
unsigned char one[4] = {'1','1','1','1'};
unsigned char two[4] = {'2','A','B','C'};
unsigned char three[4] = {'3','D','E','F'};
unsigned char four[4] = {'4','G','H','I'};
unsigned char five[4] = {'5','J','K','L'};
unsigned char six[4] = {'6','M','N','O'};
unsigned char seven[4] = {'7','P','R','S'};
unsigned char eight[4] = {'8','T','U','V'};
unsigned char nine[4] = {'9','W','X','Y'};
unsigned char zero[4] = {'0','0','0','0'};
unsigned char star[4] = {'*','*','*','*'};
unsigned char pound[4] = {'#','#','#','#'};

unsigned char previous_key_ANP = '\0';
unsigned char tmp_pressed_key;
unsigned char pushed_key_ANP;
unsigned int num_times_key_pushed = 0;
unsigned long auto_timer = 0;

unsigned char testing_flag = 0;

unsigned char AlphaNumPad(){
	/*
	unsigned char tens = 0;
	unsigned char ones = 0;
	unsigned long temp_timer = 0;
	*/
	while((pushed_key_ANP = GetKeypadKey()) == '\0'){
		if(auto_timer == 50){
			auto_timer = 0;
			return '\0';
		}
		
		/*
		temp_timer = auto_timer;
		
		tens = temp_timer / 10;
		ones = temp_timer % 10;
		
		
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_char(tens + '0',1);
		nokia_lcd_set_cursor(8,30);
		nokia_lcd_write_char(ones + '0',1);
		nokia_lcd_render();
		
		
		*/
		
		_delay_ms(100);
		++auto_timer;
	}
	
	while((tmp_pressed_key = GetKeypadKey()) == pushed_key_ANP){}
	
	auto_timer = 0;
	
	if(previous_key_ANP == pushed_key_ANP){
		if(num_times_key_pushed < 3){
			num_times_key_pushed++;
		}
		
		else{
			num_times_key_pushed = 0;
		}
		
		if(pushed_key_ANP == '1'){ return one[num_times_key_pushed]; }
		else if(pushed_key_ANP == '2'){ return (two[num_times_key_pushed]); }
		else if(pushed_key_ANP == '3'){ return (three[num_times_key_pushed]); }
		else if(pushed_key_ANP == '4'){ return (four[num_times_key_pushed]); }
		else if(pushed_key_ANP == '5'){ return (five[num_times_key_pushed]); }
		else if(pushed_key_ANP == '6'){ return (six[num_times_key_pushed]); }
		else if(pushed_key_ANP == '7'){ return (seven[num_times_key_pushed]); }
		else if(pushed_key_ANP == '8'){ return (eight[num_times_key_pushed]); }
		else if(pushed_key_ANP == '9'){ return (nine[num_times_key_pushed]); }
		else if(pushed_key_ANP == '0'){return (zero[num_times_key_pushed]); }
		else if(pushed_key_ANP == '*'){return (star[num_times_key_pushed]); }
		else if(pushed_key_ANP == '#'){return (pound[num_times_key_pushed]); }
		
	}
	
	else{
		previous_key_ANP = pushed_key_ANP;
		num_times_key_pushed = 0;
	}
	
	return previous_key_ANP;
}

////////// End of helper function that returns Numeric & AlphaNumeric Values //////////


void adduser_name_display(const char* str){
	nokia_lcd_clear();
	nokia_lcd_write_string("Enter Name",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(str,1);
	nokia_lcd_render();
}

//enum AddUserState {username, userweight, usergender, userpassword, userconfirm} adduser_state;
enum AddUserState {username,userweight} adduser_state;
char user_name[14] = "_";
unsigned char user_name_size = 0;	
unsigned char keypad_character = '\0';
unsigned char previous_character = '\0';

void AddUser_Init(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Enter Name",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(user_name,1);
	nokia_lcd_render();
	adduser_state = username;
}

void AddUser_Tick(){
	keypad_character = AlphaNumPad();
	previous_character = '\0';
	//Actions
	switch(adduser_state){
		case username:
			while(keypad_character != '#'){
				if(keypad_character == '*'){
					if(user_name_size > 0){
						--user_name_size;
						user_name[user_name_size] = '_';
						user_name[user_name_size + 1] = '\0';
						previous_character = '\0';
						adduser_name_display(user_name);
					}
					
					else{
						adduser_name_display(user_name);
					}
				}
				
				else if(keypad_character != '\0'){
					user_name[user_name_size] = keypad_character;
					user_name[user_name_size + 1] = '\0';
					previous_character = keypad_character;
					adduser_name_display(user_name);
				}
				
				else{
					if(previous_character !='\0'){
						if(user_name_size < 10){
							++user_name_size;
							user_name[user_name_size] = '_';
							user_name[user_name_size + 1] = '\0';
							previous_character = '\0';	
							adduser_name_display(user_name);
						}
					}
				}
				keypad_character = AlphaNumPad();
			}
			
			break;
		
		case userweight:	
			if(!testing_flag){
				user_name[user_name_size] = '\0';
				nokia_lcd_clear();
				nokia_lcd_write_string("Your Name is:",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string(user_name,1);
				nokia_lcd_render();
				
				testing_flag = 1;
			}
			break;
		/*
		case usergender:
			break;
			
		case userpassword:
			break;
			
		case userconfirm:
			break;
		*/
		default:
			break;
	}
	
	//Transitions
	switch(adduser_state){
		case username:
			if(keypad_character == '#'){
				adduser_state = userweight;
			}
			
			break;
		
		case userweight:
			adduser_state = userweight;
			break;
		/*
		case usergender:
			break;
		
		case userpassword:
			break;
		
		case userconfirm:
			break;
		*/
		default:
			adduser_state = username;
			break;
	}
}

void AddUserTask()
{
	AddUser_Init();
	for(;;)
	{
		AddUser_Tick();
		vTaskDelay(100);
	}
}

void AddUserPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(AddUserTask, (signed portCHAR *)"AddUserTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRC = 0x0F;	//Set PC7...PC3 to input, PC2...PC0 to output [0000 1111]
	PORTC = 0xF0;	//Init port C to 1s							[1111 0000]
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 0s
	nokia_lcd_init();
    //Start Tasks
    AddUserPulse(1);
    //RunSchedular
    vTaskStartScheduler();
}

