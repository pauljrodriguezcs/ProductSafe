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

#include "displayout.h"
#include "keypad.h"
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h"

unsigned char system_init;
unsigned char main_menu_init;
unsigned char main_menu_selection = '\0';
unsigned char temp_menu_selection = '\0';
unsigned char settings_menu_selection = '\0';
unsigned char drink_menu_selection = '\0';
unsigned char user_menu_selection = '\0';
unsigned char adding_user_flag = 0;
unsigned char removeusers_flag = 0;
unsigned char number_of_users = 0;

//// Global lock signal variables and lock sensor variables
unsigned char liquor_door_signal = 0;	// 0 = lock, 1 = unlock	PB1
unsigned char liquor_door_sensor = 0;	// 0 = door open, 1 = door closed PA1
unsigned char key_door_signal = 0;		// 0 = lock, 1 = unlock PB0
unsigned char key_door_sensor = 0;		// 0 = door open, 1 = door closed PA0
//// Global lock signal variables and lock sensor variables

struct User{
	char name[14];
	unsigned int weight;
	unsigned char gender;
	char password[9];

};

struct User List_of_Users[4] = {	{"_", 0, 0, "00000000"},
									{"_", 0, 0, "00000000"},
									{"_", 0, 0, "00000000"},
									{"_", 0, 0, "00000000"},	};
//// Add a drink globals

unsigned char add_drink_flag = 0;
unsigned char add_drink_selection = '\0';
unsigned char add_drink_prev_select = '\0';
unsigned char type_of_drink = 0; //0=no drink,1=beer, 2=wine,3=liquor,4=other
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

//// Add a drink globals

//// Replace a drink globals
unsigned char keys_in_safe_flag = 0;	// 0 = false, 1 = true;

unsigned char replace_drink_flag = 0;

//// Replace a drink globals

//// Remove users global functions

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

//// Remove users global functions

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

unsigned char AlphaNumPad(){
	while((pushed_key_ANP = GetKeypadKey()) == '\0'){
		if(auto_timer == 40){
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

////////// Menu State Machine //////////
enum MainMenuState {MainMenu, get_drink, get_key, settings, drink_options, 
					add_drink, replace_drink, user_options, add_user, 
					remove_user, reset_password, system_options} mainmenu_state;

void MainMenu_Init(){
	mainmenu_state = MainMenu;
	main_menu_display();
}

void MainMenu_Tick(){
	//Actions
	switch(mainmenu_state){
		unsigned char pushed_key = GetKeypadKey();

		case MainMenu:
			main_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(main_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
		
		case get_drink:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case get_key:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case settings:
			settings_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(settings_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case drink_options:
			drink_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(drink_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case add_drink:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case replace_drink:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case user_options:
			user_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(user_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case add_user:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case remove_user:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case reset_password:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		case system_options:
			temp_menu_selection = GetKeypadKey();
			pushed_key = GetKeypadKey();
			
			while(temp_menu_selection == pushed_key){
				pushed_key = GetKeypadKey();
			}
			
			break;
			
		default:
			break;
	}
	//Transitions
	switch(mainmenu_state){
		case MainMenu:
			if(main_menu_selection == '1'){
				mainmenu_state = get_drink;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Drink stuff*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			
			else if(main_menu_selection == '2'){
				mainmenu_state = get_key;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Key Stuff*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			
			else if(main_menu_selection == '3'){
				mainmenu_state = settings;
				settings_submenu_display();
			}
			
			else{
				mainmenu_state = MainMenu;
			}
			
			break;
		
		case get_drink:
			if(temp_menu_selection == '#'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = get_drink;
			}
			
			break;
		
		case get_key:
			if(temp_menu_selection == '#'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = get_key;
			}
			
			break;
			
		case settings:
			if(settings_menu_selection == '1'){
				mainmenu_state = drink_options;
				drinks_submenu_display();
			}
			
			else if(settings_menu_selection == '2'){
				mainmenu_state = user_options;
				user_submenu_display();
			}
			
			else if(settings_menu_selection == '3'){
				mainmenu_state = system_options;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Sttngs stuff*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			
			else if(settings_menu_selection == '4'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = settings;
			}
			
			break;
			
		case drink_options:
			if(drink_menu_selection == '1'){
				add_drink_flag = 1;
				mainmenu_state = add_drink;
				
			}
			
			else if(drink_menu_selection == '2'){
				replace_drink_flag = 1;
				mainmenu_state = replace_drink;
			}
			
			else if(drink_menu_selection == '3'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = drink_options;
			}
			
			break;
			
		case add_drink:
			if(add_drink_flag){
				mainmenu_state = add_drink;
			}
			
			else{
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			break;
			
		case replace_drink:
			if(replace_drink_flag){
				mainmenu_state = replace_drink;
			}
			
			else{
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			break;
			
		case user_options:
			if(user_menu_selection == '1'){
				mainmenu_state = add_user;
				adding_user_flag = 1;
				adduser_name_display("_");
			}
			
			else if(user_menu_selection == '2'){
				mainmenu_state = remove_user;
				removeusers_flag = 1;
				removeusers_intro();
			}
			
			else if(user_menu_selection == '3'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = user_options;
			}
			
			break;
			
		case add_user:
			if(adding_user_flag){
				mainmenu_state = add_user;
			}
			
			else{
				main_menu_display();
				mainmenu_state = MainMenu;
			}
			
			break;
			
		case remove_user:
			if(removeusers_flag){
				mainmenu_state = remove_user;
			}
			
			else{
				main_menu_display();
				mainmenu_state = MainMenu;
			}
			
			break;
			
		case reset_password:
			if(temp_menu_selection == '#'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = reset_password;
			}
			
			break;
			
		case system_options:
			if(temp_menu_selection == '#'){
				mainmenu_state = MainMenu;
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
			}
			
			else{
				mainmenu_state = system_options;
			}

			break;
		
		default:
			mainmenu_state = MainMenu;
			main_menu_selection = '\0';
			temp_menu_selection = '\0';
			settings_menu_selection = '\0';
			drink_menu_selection = '\0';
			user_menu_selection = '\0';
			main_menu_display();
			break;
	}
}

void MainMenuTask()
{
	MainMenu_Init();
    for(;;) 
    { 	
		MainMenu_Tick();
		vTaskDelay(10); 
   } 
}

void MainMenuPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(MainMenuTask, (signed portCHAR *)"MainMenuTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

////////// Menu State Machine //////////
 
 
////////// Adding A User State Machine //////////
enum AddUserState {adduser_init,username,userweight,usergender,userpassword,confirmpassword,adduser_finished} adduser_state;

char user_name[14] = "_";
char weight_output[5] = "_";
unsigned char user_name_size = 0;
unsigned int user_weight = 0;
unsigned char user_gender = 0;
char user_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char user_compare_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
unsigned char password_size = 0;
unsigned char keypad_character = '\0';
unsigned char previous_character = '\0';
unsigned char password_attempt_fails = 0;
unsigned char adding_user_finished = 0;

unsigned char password_verification(){
	for(unsigned char i = 0; i < 8; ++i){
		if(user_password[i] != user_compare_password[i]){
			return 0;
		}
	}
	
	return 1;
}

void AddUser_Init(){
	adduser_state = adduser_init;
}

void AddUser_Tick(){
	
	//Actions
	switch(adduser_state){
		case adduser_init:
			break;
			
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
						if(user_name_size < 9){
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
						nokia_lcd_write_string("Password:",1);
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
			while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
			
			while(keypad_character != '#'){}
			
			adding_user_finished = 1;
			
			break;
		
		default:
			break;
	}
	
	//Transitions
	switch(adduser_state){
		case adduser_init:
			if(adding_user_flag ){
				if(number_of_users == 4){
					adding_user_flag = 0;
					adduser_state = adduser_init;
				}
				
				else{
					for(unsigned char i = 0; i < 14; ++i){
						user_name[i] = '\0';
						if(i < 5){
							weight_output[i] = '\0';
						}
					}
					user_name[0] = '_';
					weight_output[0] = '_';
					user_name_size = 0;
					user_weight = 0;
					user_gender = 0;
				
					for(unsigned char i = 0; i < 8; ++i){
						user_password[i] = '\0';
						user_compare_password[i] = '\0';
					}
					user_password[0] = '*';
					user_compare_password[0] = '\0';
					password_size = 0;
					keypad_character = '\0';
					previous_character = '\0';
					password_attempt_fails = 0;
					adding_user_finished = 0;
				
					nokia_lcd_clear();
					nokia_lcd_write_string("Enter Name",1);
					nokia_lcd_set_cursor(0,10);
					nokia_lcd_write_string(user_name,1);
					nokia_lcd_render();
					adduser_state = username;
				}
				
			}
			
			else{
				adduser_state = adduser_init;
			}
			break;
		
		case username:
			if(keypad_character == '#' && user_name_size > 0 && adding_user_flag){
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
		
			else if(user_name_size == 0 && adding_user_flag){
				adduser_name_display("_");
				adduser_state = username;
			}
			
			else if(!adding_user_flag){
				adduser_state = adduser_init;
			}
		
			break;
		
		case userweight:
			if(user_weight > 90 && adding_user_flag){
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
		
			else if(user_weight < 90 && adding_user_flag){
				user_weight = 0;
				adduser_state = userweight;
			}
			
			else if(!adding_user_flag){
				adduser_state = adduser_init;
			}
		
			break;
		
		case usergender:
			if(user_gender != 0 && adding_user_flag){
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
		
			else if(user_gender == 0 && adding_user_flag){
				adduser_state = usergender;
			}
			
			else if(!adding_user_flag){
				adduser_state = adduser_init;
			}
		
			break;
		
		case userpassword:
			if(password_size == 8 && adding_user_flag){
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
			else if(password_size != 8 && adding_user_flag){
			
				adduser_state = userpassword;
			}
			
			else if(!adding_user_flag){
				adduser_state = adduser_init;
			}
		
			break;
		
		case confirmpassword:
			if(password_attempt_fails == 0 && adding_user_flag){
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
		
			else if(password_attempt_fails == 3 && adding_user_flag){
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
		
			else if(password_attempt_fails != 0 && password_attempt_fails != 3 && adding_user_flag){
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
			
			else if(!adding_user_flag){
				adduser_state = adduser_init;
			}
		
		
			break;
		
		case adduser_finished:
			if(adding_user_finished){
				adding_user_flag = 0;
				strncpy(List_of_Users[number_of_users].name,user_name,sizeof(List_of_Users[number_of_users].name));
				strncpy(List_of_Users[number_of_users].password,user_password,sizeof(List_of_Users[number_of_users].password));
				List_of_Users[number_of_users].gender = user_gender;
				List_of_Users[number_of_users].weight = user_weight;
				++number_of_users;
				adduser_state = adduser_init;
			}
			
			else{
				adduser_state = adduser_finished;
			}
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

 ////////// Adding A User State Machine //////////
 
 ////////// Removing A User State Machine //////////

 unsigned char removeuser_selection = '\0';
 unsigned char removeuser_prev_selection = '\0';
 unsigned char removeuser_password_fails = 0;
 char temporary_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
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

 unsigned char remove_password_verification(){
	 for(unsigned char i = 0; i < 8; ++i){
		 if(List_of_Users[user_to_remove].password[i] != temporary_password[i]){
			 return 0;
		 }
	 }
	 
	 return 1;
 }

 enum RemoveUsers {removeusers_init, removeuser_select, removeuser_password, removeuser_confirm} removeusers;

 void RemoveUsers_Init(){
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
			 
			 
			 if(!remove_password_verification()){
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
					 removeuser_selection = '\0';
					 removeuser_prev_selection = '\0';
					 removeuser_password_fails = 0;
					 user_to_remove = 5;
					 removeusers = removeuser_select;
				 }
			 
				 else{
					 removeusers_flag = 0;
					 removeusers = removeusers_init;
				 }
			 }
		 
			 else{
				 removeusers = removeusers_init;
			 }
		 
			 break;
		 
		 case removeuser_select:
			 if(removeuser_selection < number_of_users && removeusers_flag){
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
			 
			 else if(!removeusers_flag){
				 removeuser_password_fails = 0;
				 password_size = 0;
				 for(unsigned char i = 0; i < 8; ++i){
					 temporary_password[i] = '\0';
				 }
				 temporary_password[0] = '\0';
				 main_menu_display();
				 removeusers = removeusers_init;
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
				 removeuser_password_fails = 0;
				 password_size = 0;
				 for(unsigned char i = 0; i < 8; ++i){
					 temporary_password[i] = '\0';
				 }
				 temporary_password[0] = '\0';
				 removeusers_flag = 0;
				 main_menu_display();
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
				 removeuser_password_fails = 0;
				 password_size = 0;
				 for(unsigned char i = 0; i < 8; ++i){
					 temporary_password[i] = '\0';
				 }
				 temporary_password[0] = '\0';
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
			 
				 
				 removeuser_password_fails = 0;
				 password_size = 0;
				 for(unsigned char i = 0; i < 8; ++i){
					 temporary_password[i] = '\0';
				 }
				 temporary_password[0] = '\0';
				 removeusers_flag = 0;
				 main_menu_display();
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
 
 
////////// Removing A User State Machine //////////
 
////////// Adding a drink State Machine //////////
  
enum AddDrinkState {add_drink_init,add_drink_wait,drink_exists,drink_type,drink_volume,drink_ac,drink_confirm} add_drink_state;

void AddDrink_Init(){
	add_drink_state = add_drink_init;
}

void AddDrink_Tick(){
	//Actions
	switch(add_drink_state){
		case add_drink_init:
			break;
		case add_drink_wait:
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
					nokia_lcd_write_string("Can't add     anymore drinksPress # to    continue",1);
					nokia_lcd_render();
					add_drink_state = drink_exists;
				}
			
				else{
					nokia_lcd_clear();
					nokia_lcd_write_string("Close door to continue",1);
					nokia_lcd_render();
					PORTB = SetBit(PORTB,1,1);
					add_drink_state = add_drink_wait;
				}
			}
			break;
			
		case add_drink_wait:
			liquor_door_sensor = GetBit(~PINA, 1);
			
			if(liquor_door_sensor){
				PORTB = SetBit(PORTB,1,0);
				add_drink_display();
				add_drink_variable_reset();
				add_drink_state = drink_type;
			}
			
			else{
				add_drink_state = add_drink_wait;
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
  
////////// Adding a drink State Machine //////////

////////// Replace a drink State Machine //////////

void replace_drink_error(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Remove keys   from safe.    Press any key to continue.",1);
	nokia_lcd_render();
}

void replace_drink_success(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Replace drink.",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("Then close thedoor to       continue.",1);
	nokia_lcd_render();
}

enum ReplaceDrinkState {replace_init,replace_fail,replace_no_drink,replace_add_drink} replace_drink_state;

void ReplaceDrink_Init(){
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
					add_drink_flag = 1;
					type_of_drink = 0; //0=nodrink,1=beer, 2=wine,3=liquor,4=other
					volume_of_drink = 0;
					drink_alcohol_content = 0;
					add_drink_variable_reset();
					replace_drink_state = replace_add_drink;
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
		
		case replace_add_drink:
			if(add_drink_flag){
				replace_drink_state = replace_add_drink;
			}
		
			else{
				replace_drink_flag = 0;
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

////////// Replace a drink State Machine //////////
 
 
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
	DDRB = 0xFF;	//Controls the locks
	PORTB = 0x00;
	DDRA = 0x00;	//Controls the sensors to make sure door is locked
	PORTA = 0xFF;
	nokia_lcd_init();
	
	PORTB = 0x00;	// Lock all doors
	
    //Start Tasks  
    MainMenuPulse(1);
	AddUserPulse(1);
	RemoveUsersPulse(1);
	AddDrinkPulse(1);
	ReplaceDrinkPulse(1);
    //RunSchedular 
    vTaskStartScheduler(); 
	return 0; 
}