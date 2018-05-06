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

unsigned char user_to_remove = 2;
unsigned char number_of_users = 4;
unsigned char type_of_drink = 3;
unsigned char liquor_door_signal = 1;	// 0 = lock, 1 = unlock	PB1
unsigned char liquor_door_sensor = 0;	// 0 = door open, 1 = door closed PA1
unsigned char key_door_signal = 0;		// 0 = lock, 1 = unlock PB0
unsigned char key_door_sensor = 0;		// 0 = door open, 1 = door closed PA0

uint16_t volume_of_drink = 750;
unsigned char drink_alcohol_content = 40;

unsigned char get_key_flag = 1;
unsigned char current_user = 2;			// user that is using the system;
unsigned char timer_is_on = 0;				// timer that will be turned on if not sober
uint16_t current_timer = 0;				// keeps track of elapsed time

const double male_constant = 0.68;
const double female_constant = 0.55;



unsigned char drink_has_been_removed = 1;
unsigned char keys_have_been_deposited = 1;

struct User{
	char name[14];
	unsigned int weight;
	unsigned char gender;
	unsigned char key_weight;
	char password[9];

};

struct User List_of_Users[4] = {	{"JENNY", 140, 2, 7, "12345678"},
									{"CARLOS", 175, 1, 7, "87654321"},
									{"KYLE", 220, 1, 10, "12345678"},
									{"MARIA", 130, 2, 10, "87654321"},	};

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

unsigned char user_verify_flag = 0;
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

unsigned char get_drink_flag = 0;
unsigned char gd_selection = '\0';
unsigned char gd_prev_select = '\0';
unsigned char correct_key = 2;	//0 = false, 1 = true, 2 = no_selection

enum GetDrinkState {gd_init,gd_drink_removed,gd_no_drink,gd_no_user,gd_user_select,gd_deposit_keys,gd_close_door} get_drink_state;
	
void GetDrink_Init(){
	get_drink_state = gd_init;
}

void GetDrink_Tick(){
	//Actions
	switch(get_drink_state){
		case gd_init:
			break;
		
		case gd_drink_removed:
			while((gd_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gd_prev_select = GetKeypadKey()) == gd_selection){ _delay_ms(200); }
			break;
		
		case gd_no_drink:
			while((gd_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gd_prev_select = GetKeypadKey()) == gd_selection){ _delay_ms(200); }
			break;
			
		case gd_no_user:
			while((gd_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gd_prev_select = GetKeypadKey()) == gd_selection){ _delay_ms(200); }
			break;
		
		case gd_user_select:
			break;
			
		case gd_deposit_keys:
			if(GetBit(~PINA,3)){
				if(List_of_Users[user_to_remove].key_weight == 7){
					correct_key = 1;
				}
				
				else{
					correct_key = 0;
				}
			}
			
			else if(GetBit(~PINA,2)){
				if(List_of_Users[user_to_remove].key_weight == 10){
					correct_key = 1;
				}
				
				else{
					correct_key = 0;
				}
			}
			
			key_door_sensor = GetBit(~PINA,0);
			break;
			
		case gd_close_door:
			key_door_sensor = GetBit(~PINA,0);
			break;
		
		default:
			break;
	}
	
	//Transitions
	switch(get_drink_state){
		case gd_init:
			if(get_drink_flag){
				if(number_of_users == 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("No users in   system. Press any key to go to Main Menu.",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_drink_state = gd_no_user;
				}
				
				else if(type_of_drink == 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("No drinks     found. Press  any key to go to Main Menu.",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_drink_state = gd_no_drink;
				}
				
				else if(drink_has_been_removed){
					nokia_lcd_clear();
					nokia_lcd_write_string("Drink has beenremoved. Pressany key to go to Main Menu",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_drink_state = gd_no_drink;
				}
				
				else{
					correct_user_credentials = 0;
					user_verify_flag = 1;
					get_drink_state = gd_user_select;
				}
				
			}
			
			else{
				get_drink_state = gd_init;
			}
			
			break;
		
		case gd_drink_removed:
			if(gd_selection != '\0'){
				nokia_lcd_clear();
				nokia_lcd_write_string("Main Menu",1);
				nokia_lcd_render();
				get_drink_flag = 0;
				get_drink_state = gd_init;
			}
			
			else{
				get_drink_state = gd_drink_removed;
			}
			break;
			
		case gd_no_user:
			if(gd_selection != '\0'){
				nokia_lcd_clear();
				nokia_lcd_write_string("Main Menu",1);
				nokia_lcd_render();
				get_drink_flag = 0;
				get_drink_state = gd_init;
			}
			
			else{
				get_drink_state = gd_no_user;
			}
			break;	
			
		case gd_no_drink:
			if(gd_selection != '\0'){
				nokia_lcd_clear();
				nokia_lcd_write_string("Main Menu",1);
				nokia_lcd_render();
				get_drink_flag = 0;
				get_drink_state = gd_init;
			}
			
			else{
				get_drink_state = gd_no_drink;
			}
			
			break;
		
		case gd_user_select:
			if(user_verify_flag){
				get_drink_state = gd_user_select;
			}
			
			else if(!user_verify_flag && correct_user_credentials){
				if(current_user == 5){
					current_user = user_to_remove;
				}
				nokia_lcd_clear();
				nokia_lcd_write_string("Deposit Keys",1);
				nokia_lcd_render();
				correct_key = 2;
				key_door_signal = 1;
				PORTB = SetBit(PORTB,0,key_door_signal);
				get_drink_state = gd_deposit_keys;
			}
			
			else{
				// get_drink_flag = 0;
				if(!get_key_flag){
					current_user = 5;
				}
				
				correct_user_credentials = 0;
				get_drink_state = gd_init;
			}
			
			break;
			
		case gd_deposit_keys:
			if(correct_key == 2){
				get_drink_state = gd_deposit_keys;
			}
			
			else if(correct_key == 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Incorrect Keys",1);
				nokia_lcd_render();
				correct_user_credentials = 0;
				// get_drink_flag = 0;
				get_drink_state = gd_init;
			}
			
			else if(correct_key == 1){				
				if(!key_door_sensor){
					nokia_lcd_clear();
					nokia_lcd_write_string("Close Key Door",1);
					nokia_lcd_render();
				}
				
				get_drink_state = gd_close_door;
			}
			
			else{
				get_drink_state = gd_deposit_keys;
			}
			
			break;
			
		case gd_close_door:
			if(key_door_sensor){
				nokia_lcd_clear();
				nokia_lcd_write_string("Enjoy!",1);
				nokia_lcd_render();
				drink_has_been_removed = 1;
				key_door_signal = 0;
				PORTB = SetBit(PORTB,0,key_door_signal);
				liquor_door_signal = 1;
				PORTB = SetBit(PORTB,1,liquor_door_signal);
				// get_drink_flag = 0;
				get_drink_state = gd_init;
			}
			
			else{
				get_drink_state = gd_close_door;
			}
		
			break;
		
		default:
			get_drink_state = gd_init;
			break;
	}
}

void GetDrinkTask(){
	
	GetDrink_Init();
	for(;;)
	{
		GetDrink_Tick();
		vTaskDelay(100);
	}
}

void GetDrinkPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(GetDrinkTask, (signed portCHAR *)"GetDrinkTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

unsigned char gk_selection = '\0';
unsigned char gk_prev_select = '\0';
unsigned char amount_alcohol_consumed = 0;


enum GetKeyState {gk_init,gk_no_user, gk_no_drink, gk_timer, gk_user, gk_door, gk_math, gk_unlock_key, gk_timer_init} get_key_state;
	
void GetKey_Init(){
	get_key_state = gk_init;
}

void GetKey_Tick(){
	//Actions
	switch(get_key_state){
		case gk_init:
			break;
			
		case gk_no_user:
			while((gk_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gk_prev_select = GetKeypadKey()) == gk_selection){ _delay_ms(200); }
			break;
			
		case gk_no_drink:
			while((gk_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gk_prev_select = GetKeypadKey()) == gk_selection){ _delay_ms(200); }
			break;
			
		case gk_timer:
			while((gk_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gk_prev_select = GetKeypadKey()) == gk_selection){ _delay_ms(200); }
			break;
		
		case gk_user:
			break;
			
		case gk_door:
			if(GetBit(~PINA,1)){
				liquor_door_sensor = 1;
			}
			
			if(GetBit(~PINA,5)){
				drink_has_been_removed = 0;
				amount_alcohol_consumed = 100;
			}
			
			else if(GetBit(~PINA,4)){
				drink_has_been_removed = 0;
				amount_alcohol_consumed = 200;
			}
			
			else{
				drink_has_been_removed = 1;
				liquor_door_sensor = 0;
			}
			
			
			break;
			
		case gk_math:
			;
			double grams_of_alcohol_consumed;
			grams_of_alcohol_consumed =  (0.789 * (amount_alcohol_consumed * .033814) * (double)drink_alcohol_content);
			grams_of_alcohol_consumed = grams_of_alcohol_consumed / 100;
			
			double weight_grams;
			weight_grams = (double)List_of_Users[current_user].weight * 454.0;
			
			double bac = 0.0;
			
			if(List_of_Users[current_user].gender == 1){
				bac = (grams_of_alcohol_consumed/(weight_grams * male_constant)) * 1000000;
			}
			
			else{
				bac = (grams_of_alcohol_consumed/(weight_grams * female_constant)) * 1000000;
			}
			
		
			if(bac > (List_of_Users[current_user].weight * .08)){
				timer_is_on = 1;
			}
			
			else{
				timer_is_on = 0;
			}
			break;
			
		case gk_unlock_key:
			while((gk_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gk_prev_select = GetKeypadKey()) == gk_selection){ _delay_ms(200); }
			break;
			
		case gk_timer_init:
			while((gk_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((gk_prev_select = GetKeypadKey()) == gk_selection){ _delay_ms(200); }
			break;
			
		default:
			break;
	}
	
	//Transitions
	switch(get_key_state){
		case gk_init:
			if(get_key_flag){
				if(number_of_users == 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("No users in   system. Press any key to go to Main Menu.",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_key_state = gk_no_user;
				}
				
				else if(type_of_drink == 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("No drinks     found. Press  any key to go to Main Menu.",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_key_state = gk_no_drink;
				}
				
				else if(!keys_have_been_deposited){
					nokia_lcd_clear();
					nokia_lcd_write_string("No keys in system. Pressany key to go to Main Menu",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_key_state = gk_no_drink;
				}
				
				else if(timer_is_on){
					nokia_lcd_clear();
					nokia_lcd_write_string("Time Remaining",1);
					nokia_lcd_render();
					user_verify_flag = 0;
					get_key_state = gk_timer;
				}
				
				else{
					user_verify_flag = 1;
					get_key_state = gk_user;
				}
				
			}
			
			else{
				get_key_state = gk_init;
			}
			break;
			
		case gk_no_user:
			if(gk_selection != '\0'){
				nokia_lcd_clear();
				nokia_lcd_write_string("Main Menu",1);
				nokia_lcd_render();
				get_key_flag = 0;
				get_key_state = gk_init;
			}
			
			else{
				get_key_state = gk_no_user;
			}
			break;
			
		case gk_no_drink:
			if(gk_selection != '\0'){
				get_key_flag = 0;
				get_key_state = gk_init;
			}
			
			else{
				get_key_state = gk_no_drink;
			}
			break;
			
		case gk_timer:
			if(gk_selection != '\0'){
				get_key_flag = 0;
				get_key_state = gk_init;
			}
			
			else{
				get_key_state = gk_timer;
			}
			break;
			
		
		case gk_user:
			if(user_verify_flag){
				get_key_state = gk_user;
				
			}
			
			else if(!user_verify_flag && correct_user_credentials && (user_to_remove == current_user)){
				nokia_lcd_clear();
				nokia_lcd_write_string("Return Drink  and close door",1);
				nokia_lcd_render();
				get_key_state = gk_door;
			}
			
			else{
				get_key_flag = 0;
				get_key_state = gk_init;	
			}
			
			break;
		
		case gk_door:
			if(!drink_has_been_removed && liquor_door_sensor){
				get_key_state = gk_math;
			}
			
			else{
				get_key_state = gk_door;
			}
			break;
		
		case gk_math:
		
			if(timer_is_on){
				nokia_lcd_clear();
				nokia_lcd_write_string("Time Remaining",1);
				nokia_lcd_render();
				get_key_state = gk_timer_init;
				
			}
			
			else if(!timer_is_on){
				nokia_lcd_clear();
				nokia_lcd_write_string("Remove keys.  Press any key to go to Main Menu.",1);
				nokia_lcd_render();
				key_door_signal = 1;
				liquor_door_signal = 0;
				PORTB = SetBit(PORTB,0,key_door_signal);
				PORTB = SetBit(PORTB,1,liquor_door_signal);
				get_key_state = gk_unlock_key;
			}
			
			else{
				get_key_state = gk_math;
			}
			break;
		
		case gk_unlock_key:
			if(gk_selection != '\0'){
				get_key_flag = 0;
				get_key_state = gk_init;
			}
			
			else{
				get_key_state = gk_unlock_key;
			}
			break;
		
		case gk_timer_init:
			if(gk_selection != '\0'){
				get_key_flag = 0;
				get_key_state = gk_init;
			}
		
			else{
				get_key_state = gk_timer_init;
			}
			break;
		
		default:
			get_key_state = gk_init;
			break;
	}
}

void GetKeyTask(){
	GetKey_Init();
	for(;;)
	{
		GetKey_Tick();
		vTaskDelay(100);
	}
}

void GetKeyPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(GetKeyTask, (signed portCHAR *)"GetKeyTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}




int main(void)
{
	DDRC = 0x0F;	//Set PC7...PC3 to input, PC2...PC0 to output [0000 1111]
	PORTC = 0xF0;	//Init port C to 1s							[1111 0000]
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 0s
	DDRB = 0xFF;	//Controls the locks
	PORTB = 0x00;
	DDRA = 0x00;
	PORTA = 0xFF;
	
	PORTB = SetBit(PORTB,0,key_door_signal);
	PORTB = SetBit(PORTB,1,liquor_door_signal);
	
	nokia_lcd_init();
	//Start Tasks
	GetDrinkPulse(1);
	UserVerifyPulse(1);
	GetKeyPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}