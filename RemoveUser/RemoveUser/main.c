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
#include <string.h>
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

#include "keypad.h"
#include "nokia5110.h"



struct User{
	char name[14];
	unsigned int weight;
	unsigned char gender;
	char password[9];

};

struct User List_of_Users[4] = {
	{"JOHN", 145, 1, "12345678"},
	{"MARIA", 120, 2, "87654321"},
	{"ROGER", 230, 1, "43218765"},
	{"ASHLEY", 140, 2, "56781234"},
};

unsigned char removeusers_flag = 0;
unsigned char removeuser_selection = '\0';
unsigned char removeuser_prev_selection = '\0';
unsigned char removeuser_password_fails = 0;
unsigned char number_of_users = 4;
char temporary_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
unsigned char password_size = 0;
unsigned char user_to_remove = 5;

void swap_users(unsigned char dest, unsigned char source){
	char tempname[14] = " ";
	char temppass[9] = " ";
	unsigned int tempweight = 0;
	unsigned char tempgender = 0;
	
	strncpy(tempname,List_of_Users[dest].name,sizeof(tempname));
	strncpy(List_of_Users[dest].name,List_of_Users[source].name,sizeof(List_of_Users[dest].name));
	strncpy(List_of_Users[source].name,tempname,sizeof(List_of_Users[source].name));
	
	strncpy(temppass,List_of_Users[dest].password,sizeof(temppass));
	strncpy(List_of_Users[dest].password,List_of_Users[source].password,sizeof(List_of_Users[dest].password));
	strncpy(List_of_Users[source].password,temppass,sizeof(List_of_Users[source].password));
	
	tempweight = List_of_Users[dest].weight;
	List_of_Users[dest].weight = List_of_Users[source].weight;
	List_of_Users[source].weight = tempweight;
	
	tempgender = List_of_Users[dest].gender;
	List_of_Users[dest].gender = List_of_Users[source].gender;
	List_of_Users[source].gender = tempgender;
}

unsigned char password_verification(){
	for(unsigned char i = 0; i < 8; ++i){
		if(List_of_Users[user_to_remove].password[i] != temporary_password[i]){
			return 0;
		}
	}
	
	return 1;
}

void removeusers_intro(){
	nokia_lcd_clear();
	if(number_of_users == 1){
		nokia_lcd_write_string("Select User",1);
		nokia_lcd_set_cursor(0,10);
		nokia_lcd_write_string("1: ",1);
		nokia_lcd_write_string(List_of_Users[0].name,1);
		nokia_lcd_render();
	}
	
	else if(number_of_users == 2){
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
		nokia_lcd_write_string("No Users!",1);
		nokia_lcd_render();
	}
}

enum RemoveUsers {removeusers_init, removeuser_select, removeuser_password, removeuser_confirm} removeusers;

void RemoveUsers_Init(){
	removeusers_flag = 1;
	removeusers = removeusers_init;
}

void RemoveUsers_Tick(){
	//Actions
	switch(removeusers){
		case removeusers_init:
			break;
			
		case removeuser_select:
			while((removeuser_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((removeuser_prev_selection = GetKeypadKey()) == removeuser_selection){ _delay_ms(200); }
			
			removeuser_selection = removeuser_selection - '1';
					
			break;
		
		case removeuser_password:
			while((removeuser_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((removeuser_prev_selection = GetKeypadKey()) == removeuser_selection){ _delay_ms(200); }
			
			while(removeuser_selection != '#'){
				if(removeuser_selection != '\0' && removeuser_selection != '*'){
					if(password_size < 8){
						temporary_password[password_size] = removeuser_selection;
						if(password_size + 1 < 8){
							temporary_password[password_size + 1] = '*';
						}
						nokia_lcd_clear();
						nokia_lcd_write_string("Enter",1);
						nokia_lcd_set_cursor(0,10);
						nokia_lcd_write_string("Password: ",1);
						nokia_lcd_write_char(removeuser_password_fails + '0',1);
						nokia_lcd_set_cursor(0,20);
						nokia_lcd_write_string(temporary_password,1);
						nokia_lcd_render();
						
						++password_size;
					}
				}
				
				if(removeuser_selection == '*'){
					password_size = 0;
					for(unsigned char i = 0; i < 8; ++i){
						temporary_password[i] = '\0';
					}
					temporary_password[0] = '*';
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Password: ",1);
					nokia_lcd_write_char(removeuser_password_fails + '0',1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(temporary_password,1);
					nokia_lcd_render();
				}
				
				while((removeuser_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				
				while((removeuser_prev_selection = GetKeypadKey()) == removeuser_selection){ _delay_ms(200); }
				
			}
			
			
			if(!password_verification()){
				++removeuser_password_fails;
			}
			
			else{
				removeuser_password_fails = 0;
			}
			break;
		
		case removeuser_confirm:
			while((removeuser_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((removeuser_prev_selection = GetKeypadKey()) == removeuser_selection){ _delay_ms(200); }
			break;
		
		default:
			break;
	}
	//Transitions
	switch(removeusers){
		case removeusers_init:
			if(removeusers_flag){
				nokia_lcd_clear();
				if(number_of_users > 0 && number_of_users < 5){
					removeusers_intro();
					removeusers = removeuser_select;
				}
				
				else{
					nokia_lcd_write_string("No Users!",1);
					removeusers = removeusers_init;
				}
			}
			
			else{
				removeusers = removeusers_init;
			}
			
			break;
		
		case removeuser_select:
			if(removeuser_selection < number_of_users){
				user_to_remove = removeuser_selection;
				nokia_lcd_clear();
				nokia_lcd_write_string("Enter",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password: ",1);
				nokia_lcd_write_char(removeuser_password_fails + '0',1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				removeuser_password_fails = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '*';
				removeusers = removeuser_password;
			}
			
			else{
				removeusers = removeuser_select;
			}
			
			break;
		
		case removeuser_password:
			if(removeuser_password_fails == 0 && removeusers_flag){
				nokia_lcd_clear();
				nokia_lcd_write_string("Remove",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string(List_of_Users[user_to_remove].name,1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("from system?",1);
				nokia_lcd_set_cursor(0,30);
				nokia_lcd_write_string("* = no",1);
				nokia_lcd_set_cursor(0,40);
				nokia_lcd_write_string("# = yes",1);				
				nokia_lcd_render();
				removeusers = removeuser_confirm;
			}
			
			else if(removeuser_password_fails == 3 && removeusers_flag){
				removeusers_intro();
				removeuser_password_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '\0';
				
				removeusers = removeusers_init;
			}
			
			else if(removeuser_password_fails != 0 && removeuser_password_fails != 3 && removeusers_flag){
				nokia_lcd_clear();
				nokia_lcd_write_string("Enter",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password: ",1);
				nokia_lcd_write_char(removeuser_password_fails + '0',1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '*';
				removeusers = removeuser_password;
			}
			
			else if(!removeusers_flag){
				removeusers = removeusers_init;
			}
			break;
		
		case removeuser_confirm:
			if(removeuser_selection == '#'){
				if(user_to_remove == 0){
					for(unsigned char i = 0; i < 3; ++i){
						swap_users(i,i+1);
					}
					--number_of_users;
				}
				
				else if(user_to_remove == 1){
					for(unsigned char i = 1; i < 3; ++i){
						swap_users(i,i+1);
					}
					--number_of_users;
				}
				
				else if(user_to_remove == 2){
					for(unsigned char i = 2; i < 3; ++i){
						swap_users(i,i+1);
					}
					--number_of_users;
				}
				
				else if(user_to_remove == 3){
					--number_of_users;
				}
				
				removeusers_intro();
				removeuser_password_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '\0';
				
				removeusers = removeusers_init;
			}
			
			else if(removeuser_selection == '*'){
				removeusers_intro();
				removeuser_password_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					temporary_password[i] = '\0';
				}
				temporary_password[0] = '\0';
				
				removeusers = removeusers_init;
			}
			
			break;
		
		default:
			removeusers = removeusers_init;
			break;
	}
}

void RemoveUsersTask()
{
	RemoveUsers_Init();
	for(;;) 
	{ 	
		RemoveUsers_Tick();
		vTaskDelay(100); 
	} 
}

void RemoveUsersPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(RemoveUsersTask, (signed portCHAR *)"RemoveUsersTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
	DDRC = 0x0F; PORTC = 0xF0;
	DDRD = 0xFF; PORTD = 0x00;
	nokia_lcd_init();
	//Start Tasks  
	RemoveUsersPulse(1);
    //RunSchedular 
	vTaskStartScheduler(); 
 
	return 0; 
}