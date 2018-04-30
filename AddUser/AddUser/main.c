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
	while((pushed_key_ANP = GetKeypadKey()) == '\0'){
		if(auto_timer == 50){
			auto_timer = 0;
			return '\0';
		}
		
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
enum AddUserState {username,userweight, usergender, userpassword, confirmpassword, adduser_finished} adduser_state;
char user_name[14] = "_";
char weight_output[5] = "_";
unsigned char user_name_size = 0;	
unsigned char user_weight = 0;
unsigned char user_gender = 0;
char user_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char user_compare_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
unsigned password_size = 0;
unsigned char keypad_character = '\0';
unsigned char previous_character = '\0';
unsigned char password_attempt_fails = 0;

unsigned char password_verification(){
	for(unsigned char i = 0; i < 8; ++i){
		if(user_password[i] != user_compare_password[i]){
			return 0;
		}
	}
	
	return 1;
}
void AddUser_Init(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Enter Name",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(user_name,1);
	nokia_lcd_render();
	adduser_state = username;
}

void AddUser_Tick(){
	
	//Actions
	switch(adduser_state){
		case username:
			keypad_character = AlphaNumPad();
			previous_character = '\0';
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
			while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }

			while(keypad_character != '#'){
				if(keypad_character != '\0' && keypad_character != '*' && user_weight == 0){
					user_weight = keypad_character - '0';
					weight_output[0] =  keypad_character;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter your",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("weight in lbs",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(weight_output,1);
					nokia_lcd_render();
				}
				
				else if(keypad_character != '\0' && keypad_character != '*' && user_weight >= 1 && user_weight <= 9){
					user_weight = (user_weight * 10) + keypad_character - '0';
					weight_output[1] =  keypad_character;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter your",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("weight in lbs",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(weight_output,1);
					nokia_lcd_render();
				}
				
				else if(keypad_character != '\0' && keypad_character != '*' && user_weight >= 10 && user_weight <= 99){
					user_weight = (user_weight * 10) + keypad_character - '0';
					weight_output[2] = keypad_character;
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter your",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("weight in lbs",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(weight_output,1);
					nokia_lcd_render();
				}
				
				else if(keypad_character == '*'){
					user_weight = 0;
					weight_output[0] = '_';
					weight_output[1] = '\0';
					weight_output[2] = '\0';
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter your",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("weight in lbs",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(weight_output,1);
					nokia_lcd_render();
				}
				
				while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(100); }
				
				while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(100); }
				
				
			}
			break;
		
		case usergender:
			while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
			
			while(keypad_character != '#'){
				if(keypad_character == '1'){
					user_gender = 1;
					nokia_lcd_clear();
					nokia_lcd_write_string("Select Gender",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("1: Male",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string("2: Female",1);
					nokia_lcd_set_cursor(0,30);
					nokia_lcd_write_string("Male",1);
					nokia_lcd_render();
				}
				
				if(keypad_character == '2'){
					user_gender = 2;
					nokia_lcd_clear();
					nokia_lcd_write_string("Select Gender",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("1: Male",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string("2: Female",1);
					nokia_lcd_set_cursor(0,30);
					nokia_lcd_write_string("Female",1);
					nokia_lcd_render();
				}
				
				while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				
				while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
				
			}	
			
			break;
		
		case userpassword:
			while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
			
			while(keypad_character != '#'){
				if(keypad_character != '\0' && keypad_character != '*'){
					if(password_size < 8){
						user_password[password_size] = keypad_character;
						if(password_size + 1 < 8){
							user_password[password_size + 1] = '*';
						}
						nokia_lcd_clear();
						nokia_lcd_write_string("Create",1);
						nokia_lcd_set_cursor(0,10);
						nokia_lcd_write_string("Password",1);
						nokia_lcd_set_cursor(0,20);
						nokia_lcd_write_string(user_password,1);
						nokia_lcd_render();
						
						++password_size;
					}					
				}
				
				if(keypad_character == '*'){
					password_size = 0;
					for(unsigned char i = 0; i < 8; ++i){
						user_password[i] = '\0';
					}
					user_password[0] = '*';
					nokia_lcd_clear();
					nokia_lcd_write_string("Create",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Password",1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(user_password,1);
					nokia_lcd_render();
				}
				
				while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				
				while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
				
			}
						
			break;
				
		case confirmpassword:
			while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
			
			while(keypad_character != '#'){
				if(keypad_character != '\0' && keypad_character != '*'){
					if(password_size < 8){
						user_compare_password[password_size] = keypad_character;
						if(password_size + 1 < 8){
							user_compare_password[password_size + 1] = '*';
						}
						nokia_lcd_clear();
						nokia_lcd_write_string("Confirm",1);
						nokia_lcd_set_cursor(0,10);
						nokia_lcd_write_string("Password",1);
						nokia_lcd_set_cursor(60,10);
						nokia_lcd_write_char(password_attempt_fails + '0',1);
						nokia_lcd_set_cursor(0,20);
						nokia_lcd_write_string(user_compare_password,1);
						nokia_lcd_render();
						
						++password_size;
					}
					
					
				}
				
				if(keypad_character == '*'){
					password_size = 0;
					for(unsigned char i = 0; i < 8; ++i){
						user_compare_password[i] = '\0';
					}
					user_compare_password[0] = '*';
					nokia_lcd_clear();
					nokia_lcd_write_string("Confirm",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string("Password:",1);
					nokia_lcd_set_cursor(60,10);
					nokia_lcd_write_char(password_attempt_fails + '0',1);
					nokia_lcd_set_cursor(0,20);
					nokia_lcd_write_string(user_compare_password,1);
					nokia_lcd_render();
				}
				
				while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
				
				while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
				
			}
			
			
			if(!password_verification()){
				++password_attempt_fails;
			}
			
			else{
				password_attempt_fails = 0;
			}
			
			break;
		
		case adduser_finished:
			break;
			
		default:
			break;
	}
	
	//Transitions
	switch(adduser_state){
		case username:
			if(keypad_character == '#' && user_name_size > 0){
				user_name[user_name_size] = '\0';
				user_name_size = user_name_size - 1;
				user_weight = 0;
				nokia_lcd_clear();
				nokia_lcd_write_string("Enter your",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("weight in lbs",1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("_",1);
				nokia_lcd_render();
				adduser_state = userweight;
			}
			
			else if(user_name_size == 0){
				adduser_name_display("_");
				adduser_state = username;
			}
			
			break;
		
		case userweight:
			if(user_weight > 90){
				user_gender = 0;
				nokia_lcd_clear();
				nokia_lcd_write_string("Select Gender",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("1: Male",1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("2: Female",1);
				nokia_lcd_set_cursor(0,30);
				nokia_lcd_write_string("_",1);
				nokia_lcd_render();
			
				adduser_state = usergender;
			}
			
			else{
				user_weight = 0;
				adduser_state = userweight;
			}
			
			break;
		
		case usergender:
			if(user_gender != 0){
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					user_password[i] = '\0';
				}
				user_password[0] = '*';
				nokia_lcd_clear();
				nokia_lcd_write_string("Create",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password",1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				
				adduser_state = userpassword;
			}
			
			else{
				adduser_state = usergender;
			}
			
			break;
		
		case userpassword:
			if(password_size == 8){
				nokia_lcd_clear();
				nokia_lcd_write_string("Confirm",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password: ",1);
				nokia_lcd_set_cursor(60,10);
				nokia_lcd_write_char(password_attempt_fails + '0',1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				password_size = 0;
				password_attempt_fails = 0;
				for(unsigned char i = 0; i < 8; ++i){
					user_compare_password[i] = '\0';
				}
				user_compare_password[0] = '*';
				adduser_state = confirmpassword;
			}
			else{
				
				adduser_state = userpassword;
			}
			
			break;
		
		case confirmpassword:
			if(password_attempt_fails == 0){
				nokia_lcd_clear();
				nokia_lcd_write_string(user_name,1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string(weight_output,1);
				nokia_lcd_set_cursor(20,10);
				nokia_lcd_write_string("lbs",1);
				nokia_lcd_set_cursor(0,20);
				if(user_gender == 1){
					nokia_lcd_write_string("Male",1);
				}
				
				else if(user_gender == 2){
					nokia_lcd_write_string("Female",1);
				}
				
				nokia_lcd_render();
				adduser_state = adduser_finished; 
			}
				
			else if(password_attempt_fails == 3){
				nokia_lcd_clear();
				nokia_lcd_write_string("Create",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password",1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				password_attempt_fails = 0;
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					user_password[i] = '\0';
					user_compare_password[i] = '\0';
				}
				user_password[0] = '*';
				user_compare_password[0] = '\0';
				
				adduser_state = userpassword;
			}
			
			else{
				nokia_lcd_clear();
				nokia_lcd_write_string("Confirm",1);
				nokia_lcd_set_cursor(0,10);
				nokia_lcd_write_string("Password: ",1);
				nokia_lcd_set_cursor(60,10);
				nokia_lcd_write_char(password_attempt_fails + '0',1);
				nokia_lcd_set_cursor(0,20);
				nokia_lcd_write_string("*",1);
				nokia_lcd_render();
				password_size = 0;
				for(unsigned char i = 0; i < 8; ++i){
					user_compare_password[i] = '\0';
				}
				user_compare_password[0] = '*';
				adduser_state = confirmpassword;
			}
			
			
			break;
		
		case adduser_finished:
			adduser_state = adduser_finished;
			break;
		
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

