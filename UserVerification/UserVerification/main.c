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

unsigned char user_to_remove = 5;
unsigned char number_of_users = 4;

struct User{
	char name[14];
	unsigned int weight;
	unsigned char gender;
	char password[9];

};

struct User List_of_Users[4] = {	{"JENNY", 140, 2, "12345678"},
									{"CARLOS", 175, 1, "87654321"},
									{"KYLE", 220, 1, "12345678"},
									{"MARIA", 130, 2, "87654321"},	};
										

void removeusers_intro(){
	if(number_of_users == 1){
		nokia_lcd_clear();
		nokia_lcd_write_string("Select User",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_write_string("1: ",1);
		nokia_lcd_write_string(List_of_Users[0].name,1);
		nokia_lcd_render();
	}
	
	else if(number_of_users == 2){
		nokia_lcd_clear();
		nokia_lcd_write_string("Select User",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_write_string("1: ",1);
		nokia_lcd_write_string(List_of_Users[0].name,1);
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("2: ",1);
		nokia_lcd_write_string(List_of_Users[1].name,1);
		nokia_lcd_render();
	}
	
	else if(number_of_users == 3){
		nokia_lcd_clear();
		nokia_lcd_write_string("Select User",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_write_string("1: ",1);
		nokia_lcd_write_string(List_of_Users[0].name,1);
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("2: ",1);
		nokia_lcd_write_string(List_of_Users[1].name,1);
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_string("3: ",1);
		nokia_lcd_write_string(List_of_Users[2].name,1);
		nokia_lcd_render();
	}
	
	else if(number_of_users == 4){
		nokia_lcd_clear();
		nokia_lcd_write_string("Select User",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_write_string("1: ",1);
		nokia_lcd_write_string(List_of_Users[0].name,1);
		nokia_lcd_set_cursor(0,20);
		nokia_lcd_write_string("2: ",1);
		nokia_lcd_write_string(List_of_Users[1].name,1);
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_string("3: ",1);
		nokia_lcd_write_string(List_of_Users[2].name,1);
		nokia_lcd_set_cursor(0,40);
		nokia_lcd_write_string("4: ",1);
		nokia_lcd_write_string(List_of_Users[3].name,1);
		nokia_lcd_render();
	}
	
	else{
		nokia_lcd_clear();
		nokia_lcd_write_string("No users in   system. Press any key to go to Main Menu.",1);
		nokia_lcd_render();
	}
}



char temporary_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
unsigned char password_size = 0;

unsigned char remove_password_verification(){
	for(unsigned char i = 0; i < 8; ++i){
		if(List_of_Users[user_to_remove].password[i] != temporary_password[i]){
			return 0;
		}
	}
	
	return 1;
}

unsigned char user_verify_flag = 1;
unsigned char correct_user_credentials = 0;
unsigned char uv_selection = '\0';
unsigned char uv_prev_select = '\0';
unsigned char uv_password_fails = 0;
										
enum UserVerifyState {uv_init,uv_display,uv_password,} user_verify_state;

void UserVerify_Init(){
	user_verify_state = uv_init;
}

void UserVerify_Tick(){
	//Actions
	switch(user_verify_state){
		case uv_init:
			break;
		
		case uv_display:
			while((uv_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((uv_prev_select = GetKeypadKey()) == uv_selection){ _delay_ms(200); }
			
			uv_selection = uv_selection - '1';
			break;
			
		case uv_password:
			while((uv_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((uv_prev_select = GetKeypadKey()) == uv_selection){ _delay_ms(200); }
			
			while(uv_selection != '#'){
				if(uv_selection != '\0' && uv_selection != '*'){
					if(password_size < 8){
						temporary_password[password_size] = uv_selection;
						if(password_size + 1 < 8){
							temporary_password[password_size + 1] = '*';
						}
						nokia_lcd_clear();
						nokia_lcd_write_string("Enter",1);
						nokia_lcd_set_cursor(0,10);
						nokia_lcd_write_string("Password: ",1);
						nokia_lcd_write_char(uv_password_fails + '0',1);
						nokia_lcd_set_cursor(0,20);
						nokia_lcd_write_string(temporary_password,1);
						nokia_lcd_render();
						
						++password_size;
					}
				}
				
				if(uv_selection == '*'){
					password_size = 0;
					for(unsigned char i = 0; i < 8; ++i){
						temporary_password[i] = '\0';
					}
					temporary_password[0] = '*';
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Password: ",1);
					nokia_lcd_write_char(uv_password_fails + '0',1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(temporary_password,1);
					nokia_lcd_render();
				}
				
				while((uv_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				while((uv_prev_select = GetKeypadKey()) == uv_selection){ _delay_ms(200); }
			}
			
			
			if(!remove_password_verification()){
				++uv_password_fails;
			}
			
			else{
				uv_password_fails = 0;
			}
			break;
			
		default:
			break;
	}
	//Transitions
	switch(user_verify_state){
		case uv_init:
			if(user_verify_flag){
				removeusers_intro();
				user_to_remove = 5;
				correct_user_credentials = 0;
				user_verify_state = uv_display;
			}
			
			else{
				user_verify_state = uv_init;
			}
			break;
			
		case uv_display:
			if(uv_selection < number_of_users && user_verify_flag){
				user_to_remove = uv_selection;
				uv_password_fails = 0;
				nokia_lcd_clear();
				nokia_lcd_write_string("Enter",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password: ",1);
				nokia_lcd_write_char(uv_password_fails + '0',1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '*';
				user_verify_state = uv_password;
			}
			
			else if(!user_verify_flag){
				uv_password_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '\0';
				user_verify_state = uv_init;
			}
			
			else{
				user_verify_state = uv_display;
			}
			
			break;
		
		case uv_password:
			if(uv_password_fails == 0 && user_verify_flag){
				nokia_lcd_clear();
				nokia_lcd_write_string("User verified",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("successfully",1);
				nokia_lcd_render();
				correct_user_credentials = 1;
				user_verify_flag = 0;
				user_verify_state = uv_init;
			}
			
			else if(uv_password_fails == 3 && user_verify_flag){
				uv_password_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '\0';
				nokia_lcd_clear();
				nokia_lcd_write_string("User verified",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("unsuccessfully",1);
				nokia_lcd_render();
				correct_user_credentials = 0;
				user_verify_flag = 0;
				user_verify_state = uv_init;
			}
			
			else if(uv_password_fails != 0 && uv_password_fails != 3 && user_verify_flag){
				nokia_lcd_clear();
				nokia_lcd_write_string("Enter",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password: ",1);
				nokia_lcd_write_char(uv_password_fails + '0',1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '*';
				user_verify_state = uv_password;
			}
			
			else if(!user_verify_flag){
				uv_password_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '\0';
				user_verify_state = uv_init;
			}
			break;
		
		default:
			break;
	}
}

void UserVerifyTask()
{
	UserVerify_Init();
	for(;;) 
	{ 	
		UserVerify_Tick();
		vTaskDelay(100); 
	} 
}

void UserVerifyPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(UserVerifyTask, (signed portCHAR *)"UserVerifyTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
	DDRC = 0x0F;	//Set PC7...PC3 to input, PC2...PC0 to output [0000 1111]
	PORTC = 0xF0;	//Init port C to 1s							[1111 0000]
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 0s
	DDRB = 0xFF;	//Controls the locks
	PORTB = 0x00;
	nokia_lcd_init();
	//Start Tasks  
	UserVerifyPulse(1);
    //RunSchedular 
	vTaskStartScheduler(); 
 
	return 0; 
}