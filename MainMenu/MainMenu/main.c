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
#include "HX711.h"
#include "HX711_1.h"
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h"

// System flags to be used to control different state machines
unsigned char system_init;
unsigned char main_menu_init;
unsigned char main_menu_selection = '\0';
unsigned char temp_menu_selection = '\0';
unsigned char settings_menu_selection = '\0';
unsigned char drink_menu_selection = '\0';
unsigned char user_menu_selection = '\0';
unsigned char adding_user_flag = 0;
unsigned char removeusers_flag = 0;
unsigned char user_verify_flag = 0;
unsigned char get_drink_flag = 0;
unsigned char get_key_flag = 0;
unsigned char time_elapsed_flag = 0;
unsigned char correct_user_credentials = 0;
unsigned char number_of_users = 0;

// Flags to be used by the timer state machine *To be implemented in v2*
unsigned char current_user = 5;			// user that is using the system;
unsigned char timer_is_on = 0;			// timer that will be turned on if not sober
uint16_t time_elapsed_counter = 0;		// keeps track of elapsed time
uint16_t time_remaining_counter = 0;	// time remaining to unlock door

const double male_constant = 0.68;
const double female_constant = 0.55;

//// Global lock signal variables and lock sensor variables
unsigned char liquor_door_signal = 0;	// 0 = lock, 1 = unlock	PB1
unsigned char liquor_door_sensor = 0;	// 0 = door open, 1 = door closed PA1
unsigned char key_door_signal = 0;		// 0 = lock, 1 = unlock PB0
unsigned char key_door_sensor = 0;		// 0 = door open, 1 = door closed PA0

//// Drink and Key global variables
unsigned char drink_has_been_removed = 0;
unsigned char keys_have_been_deposited = 0;

// Struct holding user information
struct User{
	char name[14];
	unsigned int weight;
	unsigned char gender;
	long key_weight;
	char password[9];

};

// array of users that will be stored in eeprom *to be implemented in v2*
struct User List_of_Users[4] = {	{"_", 0, 0, 0, "00000000"},
									{"_", 0, 0, 0, "00000000"},
									{"_", 0, 0, 0, "00000000"},
									{"_", 0, 0, 0, "00000000"},	};
										
//// Add a drink globals
unsigned char add_drink_flag = 0;
unsigned char add_drink_selection = '\0';
unsigned char add_drink_prev_select = '\0';
unsigned char type_of_drink = 0; //0=no drink,1=beer, 2=wine,3=liquor,4=other
long volume_of_drink = 0;
long total_container_weight = 0;
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
unsigned char replace_drink_flag = 0;
//// Replace a drink globals

//// Remove users global functions

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
		if(auto_timer == 20){	// automatically move the cursor in the array of chars
			auto_timer = 0;
			return '\0';
		}
		
		_delay_ms(100);
		++auto_timer;
	}
	
	while((tmp_pressed_key = GetKeypadKey()) == pushed_key_ANP){}	// make sure the button has been released before continuing
	
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

////////// Scale reader functions

unsigned char alcohol_scale_reader(){
	volume_of_drink = volume_of_drink - 100;
	return 100;
}

unsigned char key_scale_reader(){
	return 10;
}



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
			main_menu_selection = GetKeypadKey();		// machine will only move along once the keypad button has been released
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
				get_drink_flag = 1;
				mainmenu_state = get_drink;
			}
			
			else if(main_menu_selection == '2'){
				get_key_flag = 1;
				mainmenu_state = get_key;

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
			if(get_drink_flag){
				mainmenu_state = get_drink;
			}
			
			else{
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
				mainmenu_state = MainMenu;
			}
			
			break;
		
		case get_key:
			if(get_key_flag){
				mainmenu_state = get_key;
			}
			
			else{
				main_menu_selection = '\0';
				temp_menu_selection = '\0';
				settings_menu_selection = '\0';
				drink_menu_selection = '\0';
				user_menu_selection = '\0';
				main_menu_display();
				mainmenu_state = MainMenu;
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
			
			/*
			else if(settings_menu_selection == '3'){
				mainmenu_state = system_options;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Sttngs stuff*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			*/
			
			else if(settings_menu_selection == '3'){
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
		
		/*	
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
		*/
		
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
enum AddUserState {adduser_init,adduser_deny,username,userweight,usergender,
					userkey,userpassword,confirmpassword,adduser_finished} adduser_state;

char user_name[14] = "_";
char weight_output[5] = "_";
unsigned char user_name_size = 0;
unsigned int user_weight = 0;
unsigned char user_gender = 0;
long user_key = 0;
char user_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char user_compare_password[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
unsigned char password_size = 0;
unsigned char keypad_character = '\0';
unsigned char previous_character = '\0';
unsigned char password_attempt_fails = 0;
unsigned char adding_user_finished = 0;

unsigned char password_verification(){		// compare the users original password with the entered password
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
			
		case adduser_deny:
			while((keypad_character = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((previous_character = GetKeypadKey()) == keypad_character){ _delay_ms(200); }
			break;
			
		case username:
			keypad_character = AlphaNumPad();
			previous_character = '\0';
			while(keypad_character != '#'){			//'#' means enter so as long as its not pressed carry on
				if(keypad_character == '*'){		//'*' means backspace
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
			
				else if(keypad_character != '\0'){		//its time to update the cursor by one
					user_name[user_name_size] = keypad_character;
					user_name[user_name_size + 1] = '\0';
					previous_character = keypad_character;
					adduser_name_display(user_name);
				}
			
				else{
					if(previous_character !='\0'){	// makes sure the cursor hasnt moved automatically 
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
			
		case userkey:
			;
			long weight_of_key;
			
			weight_of_key = HX711_1_get_units(10);
			if(weight_of_key < 0){
				weight_of_key = 0;
			}
			
			
			while(weight_of_key != 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Remove keys   from scale",1);
				
				nokia_lcd_render();
				
				weight_of_key = HX711_1_get_units(10);
				if(weight_of_key < 0){
					weight_of_key = 0;
				}
			}
			
			while(weight_of_key == 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Place keys on scale",1);
				nokia_lcd_render();
				
				weight_of_key = HX711_1_get_units(10);
				if(weight_of_key < 0){
					weight_of_key = 0;
				}
			}
			
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			
			long weight_flag;
			
			weight_flag = weight_of_key;
			
			while(weight_flag != 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Remove keys   from scale",1);
				nokia_lcd_render();
				
				weight_flag = HX711_1_get_units(10);
				if(weight_flag < 0){
					weight_flag = 0;
				}
			}
			
			user_key = weight_of_key;
			
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
			
				if(keypad_character == '*'){	// clears the user password from the screen and starts over
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
		
		
			if(!password_verification()){	// gives the user three tries to enter correct password or else it takes you back to main menu
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
			if(adding_user_flag){
				if(!keys_have_been_deposited){	// needs to make sure the system is not under use
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
					nokia_lcd_clear();
					nokia_lcd_write_string("System is in  use. Try again later",1);
					nokia_lcd_render();
					
					adduser_state = adduser_deny;
				}	
				
			}
			
			else{
				adduser_state = adduser_init;
			}
			break;
		
		case adduser_deny:
			if(keypad_character != '\0'){
				adding_user_flag = 0;
				adduser_state = adduser_init;
			}
			
			else{
				adduser_state = adduser_deny;
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
				key_door_signal = 1;
				PORTB = SetBit(PORTB,0,key_door_signal);
				nokia_lcd_clear();
				nokia_lcd_write_string("Weighing keys",1);
				nokia_lcd_render();
				user_key = 0;
				adduser_state = userkey;
			}
		
			else if(user_gender == 0 && adding_user_flag){
				adduser_state = usergender;
			}
			
			else if(!adding_user_flag){
				adduser_state = adduser_init;
			}
		
			break;
			
		case userkey:
			if(user_key > 0 && adding_user_flag){
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
			
			else if(user_key == 0 && adding_user_flag){
				adduser_state = userkey;
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
				adding_user_finished = 1;
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
				adding_user_finished = 0;
				adding_user_flag = 0;
				strncpy(List_of_Users[number_of_users].name,user_name,sizeof(List_of_Users[number_of_users].name));
				strncpy(List_of_Users[number_of_users].password,user_password,sizeof(List_of_Users[number_of_users].password));
				List_of_Users[number_of_users].gender = user_gender;
				List_of_Users[number_of_users].weight = user_weight;
				List_of_Users[number_of_users].key_weight = user_key;
				++number_of_users;
				key_door_signal = 0;
				PORTB = SetBit(PORTB,0,key_door_signal);
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

 void swap_users(unsigned char dest, unsigned char source){	// this function is to move the users along the array so they appear towards the beginning
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

 enum RemoveUsers {removeusers_init, removeuser_deny, removeuser_false,removeuser_select, removeuser_password, removeuser_confirm} removeusers;

 void RemoveUsers_Init(){
	 removeusers = removeusers_init;
 }

 void RemoveUsers_Tick(){
	 //Actions
	 switch(removeusers){
		 case removeusers_init:
			break;
			
		case removeuser_deny:
			while((removeuser_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((removeuser_prev_selection = GetKeypadKey()) == removeuser_selection){ _delay_ms(200); }
			break;
			
		case removeuser_false:
			while((removeuser_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((removeuser_prev_selection = GetKeypadKey()) == removeuser_selection){ _delay_ms(200); }
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
				 if(keys_have_been_deposited || drink_has_been_removed){
					 nokia_lcd_clear();
					 nokia_lcd_write_string("System is in  use. Try again later",1);
					 nokia_lcd_render();
					 removeusers = removeuser_deny;
				 }
				 
				 else if(number_of_users > 0 && number_of_users < 5){	//number of users needs to be within appropriate range
					 removeuser_selection = '\0';
					 removeuser_prev_selection = '\0';
					 removeuser_password_fails = 0;
					 user_to_remove = 5;
					 removeusers = removeuser_select;
				 }
			 
				 else{
					 removeusers = removeuser_false;
				 }
			 }
		 
			 else{
				 removeusers = removeusers_init;
			 }
		 
			 break;
			
		 case removeuser_deny:
			if(removeuser_selection != '\0'){
				removeusers_flag = 0;
				removeusers = removeusers_init;
			}
			
			else{
				removeusers = removeuser_deny;
			}
			
			break;
						
		 case removeuser_false:
			if(removeuser_selection != '\0'){
				removeusers_flag = 0;
				removeusers = removeusers_init;
			}
			
			else{
				removeusers = removeuser_false;
			}
			
			break;
		 
		 case removeuser_select:
			 if(removeuser_selection < number_of_users && removeusers_flag){	//a valid number has to be inputted into keypad
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
		 
			 else if(removeuser_password_fails == 3 && removeusers_flag){	//deny the user from deleting user because multiple failed attempts
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
  
enum AddDrinkState {add_drink_init, add_drink_no_user, add_drink_verify, add_drink_wait,
					drink_exists, drink_type, drink_volume, drink_ac, drink_confirm} add_drink_state;

void AddDrink_Init(){
	add_drink_state = add_drink_init;
}

void AddDrink_Tick(){
	//Actions
	switch(add_drink_state){
		case add_drink_init:
			break;
		
		case add_drink_no_user:
			while((add_drink_selection = GetKeypadKey()) == '\0'){ _delay_ms(200); }
			while((add_drink_prev_select = GetKeypadKey()) == add_drink_selection){ _delay_ms(200); }
			break;
		
		case add_drink_verify:
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
				if(type_of_drink != 0){	//the number of possible additional drinks has been exceeded
					nokia_lcd_clear();
					nokia_lcd_write_string("Can't add     anymore drinksPress # to    continue",1);
					nokia_lcd_render();
					add_drink_state = drink_exists;
				}
				
				else if(number_of_users == 0){
					nokia_lcd_clear();
					nokia_lcd_write_string("No users in   system. Press any key to go to Main Menu.",1);
					nokia_lcd_render();
					add_drink_flag = 0;
					user_verify_flag = 0;
					add_drink_state = add_drink_no_user;
				}
				else{
					user_verify_flag = 1;
					add_drink_state = add_drink_verify;
				}
				/*
				else{
					nokia_lcd_clear();
					nokia_lcd_write_string("Close door to continue",1);
					nokia_lcd_render();
					PORTB = SetBit(PORTB,1,1);
					add_drink_state = add_drink_wait;
				}
				*/
			}
			break;
			
		case add_drink_no_user:
			if(add_drink_selection != '\0'){
				add_drink_flag = 0;
				add_drink_variable_reset();
				add_drink_state = add_drink_init;
			}
			
			else{
				add_drink_state = add_drink_no_user;
			}
			break;
			
		case add_drink_verify:
			if(user_verify_flag){
				add_drink_state = add_drink_verify;
			}
			
			else if(!user_verify_flag && correct_user_credentials){
				nokia_lcd_clear();
				nokia_lcd_write_string("Close liquor  door to        continue",1);
				nokia_lcd_render();
				PORTB = SetBit(PORTB,1,1);
				correct_user_credentials = 0;
				add_drink_state = add_drink_wait;
			}
			
			else{
				add_drink_flag = 0;
				add_drink_variable_reset();
				add_drink_state = add_drink_init;
			}
			
			break;
			
		case add_drink_wait:
			liquor_door_sensor = GetBit(~PINB, 5);
			
			if(liquor_door_sensor && add_drink_flag){
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
					unsigned char tens = ((volume_of_drink % 1000) % 100) / 10;
					unsigned char ones = ((volume_of_drink % 1000) % 100) % 10;
					nokia_lcd_write_char(thousands + '0',1);
					nokia_lcd_write_char(hundreds + '0',1);
					nokia_lcd_write_char(tens + '0',1);
					nokia_lcd_write_char(ones + '0',1);
					nokia_lcd_write_string(" mL",1);
				}
			
				else{
					unsigned char hundreds = volume_of_drink / 100;
					unsigned char tens = (volume_of_drink % 100) / 10;
					unsigned char ones = (volume_of_drink % 100) % 10;
					nokia_lcd_write_char(hundreds + '0',1);
					nokia_lcd_write_char(tens + '0',1);
					nokia_lcd_write_char(ones + '0',1);
					nokia_lcd_write_string(" mL",1);
				}
			
				nokia_lcd_set_cursor(0,20);
			
				if(drink_alcohol_content > 99){
					unsigned char hundreds = drink_alcohol_content / 100;
					unsigned char tens = (drink_alcohol_content % 100) / 10;
					unsigned char ones = (drink_alcohol_content % 100) % 10;
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
				//eeprom_update_byte(&e_type_of_drink,type_of_drink);
				//eeprom_update_byte(&e_drink_alcohol_content,drink_alcohol_content);
				//eeprom_update_word(&e_volume_of_drink,volume_of_drink);
				
				total_container_weight = 0;
				total_container_weight = HX711_get_units(10);
				total_container_weight = HX711_get_units(10);
				total_container_weight = HX711_get_units(10);
				total_container_weight = HX711_get_units(10);
				total_container_weight = HX711_get_units(10);
								
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
				if(keys_have_been_deposited){
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
 
////////// User Verification Helper State Machine //////////
 
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
 
////////// User Verification Helper State Machine //////////
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
			;
			long weight_of_key;
			
			weight_of_key = HX711_1_get_units(10);
			if(weight_of_key < 0){
				weight_of_key = 0;
			}
			
			
			while(weight_of_key != 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Remove keys   from scale",1);
				
				nokia_lcd_render();
				
				weight_of_key = HX711_1_get_units(10);
				if(weight_of_key < 0){
					weight_of_key = 0;
				}
			}
			
			while(weight_of_key == 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Place keys on scale",1);
				nokia_lcd_render();
				
				weight_of_key = HX711_1_get_units(10);
				if(weight_of_key < 0){
					weight_of_key = 0;
				}
			}
			
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			weight_of_key = HX711_1_get_units(10);
			
			if((List_of_Users[user_to_remove].key_weight - 2 < weight_of_key) && (weight_of_key < List_of_Users[user_to_remove].key_weight + 2)){
				correct_key = 1;
			}
			
			else{
				correct_key = 0;
			}
			
			key_door_sensor = GetBit(~PINB,4);
			
			break;
		
		case gd_close_door:
			key_door_sensor = GetBit(~PINB,4);
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
				get_drink_flag = 0;
				get_drink_state = gd_init;
			}
		
			else{
				get_drink_state = gd_drink_removed;
			}
			break;
		
		case gd_no_user:
			if(gd_selection != '\0'){
				get_drink_flag = 0;
				get_drink_state = gd_init;
			}
		
			else{
				get_drink_state = gd_no_user;
			}
			break;
		
		case gd_no_drink:
			if(gd_selection != '\0'){
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
				if(!get_key_flag){
					current_user = 5;
				}
				get_drink_flag = 0;
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
				nokia_lcd_write_string("Incorrect Keys. Press any key to return to main menu",1);
				nokia_lcd_render();
				correct_user_credentials = 0;
				get_drink_flag = 0;
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
				keys_have_been_deposited = 1;
				nokia_lcd_clear();
				nokia_lcd_write_string("Enjoy! Press  any key to    continue",1);
				nokia_lcd_render();
				drink_has_been_removed = 1;
				key_door_signal = 0;
				PORTB = SetBit(PORTB,0,key_door_signal);
				liquor_door_signal = 1;
				PORTB = SetBit(PORTB,1,liquor_door_signal);
				get_drink_flag = 0;
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

/////// Get Key State Machine ///////

unsigned char gk_selection = '\0';
unsigned char gk_prev_select = '\0';
unsigned char amount_alcohol_consumed = 0;



enum GetKeyState {gk_init,gk_no_user, gk_no_drink, gk_user, gk_door, gk_math, gk_unlock_key, gk_timer_init} get_key_state;

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
		
	
		case gk_user:
			break;
		
		case gk_door:
			if(GetBit(~PINB,5)){
				liquor_door_sensor = 1;
				long temp_measurement = HX711_get_units(10);
				
				if((temp_measurement < total_container_weight) && ((total_container_weight - temp_measurement) < volume_of_drink)){
					drink_has_been_removed = 0;
				}
				//amount_alcohol_consumed = alcohol_scale_reader();
				//drink_weight = HX711_get_units(10);
			}
			
			/*
			if(GetBit(~PINA,5)){
				drink_has_been_removed = 0;
				amount_alcohol_consumed = 100;
				volume_of_drink = volume_of_drink - amount_alcohol_consumed;
				
				if(volume_of_drink == 0){
					type_of_drink = 0;
				}
			}
		
			else if(GetBit(~PINA,4)){
				drink_has_been_removed = 0;
				amount_alcohol_consumed = 200;
				volume_of_drink = volume_of_drink - amount_alcohol_consumed;
				
				if(volume_of_drink == 0){
					type_of_drink = 0;
				}
			}
			*/
			
			else{
				drink_has_been_removed = 1;
				liquor_door_sensor = 0;
			}
		
			break;
		
		case gk_math:
			;
			long new_container_weight;
			new_container_weight = HX711_get_units(10);
			new_container_weight = HX711_get_units(10);
			new_container_weight = HX711_get_units(10);
			new_container_weight = HX711_get_units(10);
			new_container_weight = HX711_get_units(10);
			
			amount_alcohol_consumed = total_container_weight - new_container_weight;
			total_container_weight = new_container_weight;
			volume_of_drink = volume_of_drink - amount_alcohol_consumed;
			
			if(volume_of_drink <= 0){
				type_of_drink = 0;
			}
			
			double grams_of_alcohol_consumed;
			//grams_of_alcohol_consumed =  (0.789 * (amount_alcohol_consumed * .033814) * (double)drink_alcohol_content);
			grams_of_alcohol_consumed =  (0.789 * amount_alcohol_consumed * (double)drink_alcohol_content);
			double weight_grams;
			weight_grams = (double)List_of_Users[current_user].weight * 454.0;
		
			double bac = 0.0;
		
			if(List_of_Users[current_user].gender == 1){
				bac = (grams_of_alcohol_consumed/(weight_grams * male_constant)) * 100;
			}
		
			else{
				bac = (grams_of_alcohol_consumed/(weight_grams * female_constant)) * 100;
			}
			
			double bac_elapsed_time = (double)time_elapsed_counter * 0.000004167;
			
			bac = bac - bac_elapsed_time;
			
			if(bac > .08){
				bac_elapsed_time = bac - .08;
				time_remaining_counter = ((bac_elapsed_time / 0.015) * 3600);
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
			;
			uint16_t temp_time = time_remaining_counter / 2 ;
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
			nokia_lcd_write_string("Time Remaining",1);
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
		
		case gk_user:
			if(user_verify_flag){
				get_key_state = gk_user;
			
			}
		
			else if(!user_verify_flag && correct_user_credentials && (user_to_remove == current_user)){
				amount_alcohol_consumed = HX711_get_units(10);
				amount_alcohol_consumed = HX711_get_units(10);
				amount_alcohol_consumed = HX711_get_units(10);
				amount_alcohol_consumed = HX711_get_units(10);
				amount_alcohol_consumed = HX711_get_units(10);
				
				nokia_lcd_clear();
				nokia_lcd_write_string("Return Drink  and close door",1);
				nokia_lcd_render();
				
				liquor_door_signal = 0;
		
				PORTB = SetBit(PORTB,1,liquor_door_signal);
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
				drink_has_been_removed = 0;
				keys_have_been_deposited = 0;
				get_key_state = gk_init;
			}
		
			else{
				get_key_state = gk_unlock_key;
			}
			break;
		
		case gk_timer_init:
			if(time_remaining_counter > 0){
				--time_remaining_counter;
				get_key_state = gk_timer_init;
				
			}
			
			else if(time_remaining_counter == 0){
				nokia_lcd_clear();
				nokia_lcd_write_string("Remove keys.  Press any key to go to Main Menu.",1);
				nokia_lcd_render();
				timer_is_on = 0;
				key_door_signal = 1;
				liquor_door_signal = 0;
				PORTB = SetBit(PORTB,0,key_door_signal);
				PORTB = SetBit(PORTB,1,liquor_door_signal);
				get_key_state = gk_unlock_key;
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

/////// Get Key State Machine ///////

/////// Auto-lock liquor door State Machine ///////
// This state machine gives the user a minute to remove the drink 
// and then locks the door to prevent solenoid lock from overheating

uint16_t liquor_door_counter = 0;

enum AutoLockLiquorDoor {alld_wait, alld_count, alld_lock} auto_lock_liquor_door;	
	
void AutoLockLiquorDoor_Init(){
	auto_lock_liquor_door = alld_wait;
}

void AutoLockLiquorDoor_Tick(){
	
	//Actions
	switch(auto_lock_liquor_door){
		case alld_wait:
			break;
			
		case alld_count:
			break;
			
		case alld_lock:
			break;
			
		default:
			break;
	}
	
	//Transitions
	switch(auto_lock_liquor_door){
		case alld_wait:
			if(drink_has_been_removed && liquor_door_signal){
				liquor_door_counter = 0;
				auto_lock_liquor_door = alld_count;
			}
			
			else{
				auto_lock_liquor_door = alld_wait;
			}
			
			break;
		
		case alld_count:
			if(liquor_door_counter > 3000){	// change to 6000 for 1 minute
				auto_lock_liquor_door = alld_lock;
			}
			
			else{
				++liquor_door_counter;
				auto_lock_liquor_door = alld_count;
			}
			
			break;
		
		case alld_lock:
			liquor_door_signal = 0;
			PORTB = SetBit(PORTB,1,liquor_door_signal);
			auto_lock_liquor_door = alld_wait;
			break;
		
		default:
			auto_lock_liquor_door = alld_wait;
			break;
	}
}

void AutoLockLiquorDoorTask(){
	
	AutoLockLiquorDoor_Init();
	for(;;){
		AutoLockLiquorDoor_Tick();
		vTaskDelay(10);
	}
}

void AutoLockLiquorDoorPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(AutoLockLiquorDoorTask, (signed portCHAR *)"AutoLockLiquorDoorTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

/////// Auto-lock liquor door State Machine ///////

/////// Auto-lock key door State Machine ///////

uint16_t key_door_counter = 0;
enum AutoLockKeyDoor {alkd_wait, alkd_count, alkd_lock} auto_lock_key_door;

void AutoLockKeyDoor_Init(){
	auto_lock_key_door = alkd_wait;
}

void AutoLockKeyDoor_Tick(){
	
	//Actions
	switch(auto_lock_key_door){
		case alkd_wait:
		break;
		
		case alkd_count:
		break;
		
		case alkd_lock:
		break;
		
		default:
		break;
	}
	
	//Transitions
	switch(auto_lock_key_door){
		case alkd_wait:
			if(!keys_have_been_deposited && key_door_signal){
				key_door_counter = 0;
				auto_lock_key_door = alkd_count;
			}
		
			else{
				auto_lock_key_door = alkd_wait;
			}
		
			break;
		
		case alkd_count:
			if(key_door_counter > 3000){	// change to 6000 for 1 minute
				auto_lock_key_door = alkd_lock;
			}
		
			else{
				++key_door_counter;
				auto_lock_key_door = alkd_count;
			}
		
			break;
		
		case alkd_lock:
			key_door_signal = 0;
			PORTB = SetBit(PORTB,0,key_door_signal);
			auto_lock_key_door = alkd_wait;
			break;
		
		default:
			auto_lock_key_door = alkd_wait;
			break;
	}
}

void AutoLockKeyDoorTask(){
	
	AutoLockKeyDoor_Init();
	for(;;){
		AutoLockKeyDoor_Tick();
		vTaskDelay(10);
	}
}

void AutoLockKeyDoorPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(AutoLockKeyDoorTask, (signed portCHAR *)"AutoLockKeyDoorTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}


/////// Auto-lock key door State Machine ///////

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
			break;
		
		default:
			break;
	}
	
	//Transitions
	switch(time_elapsed_state){
		case te_wait:
			if(drink_has_been_removed){
				time_elapsed_counter = 0;
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

int main(void) 
{
	/*
	when scaling by 1: 14 chars per line, 8 pixels in height
	when scaling by 2: 8 chars per line, 14 pixels in height
	when scaling by 3: 5 chars per line, 21 pixels in height
	*/
	DDRA = 0x00;	//Controls the sensors to make sure door is locked
	PORTA = 0xFF;
	DDRB = 0x0F;	//Controls the locks [keylock = PB0, liquorlock = PB1
	PORTB = 0xF0;	//Controls the door sensors [keydoor = PB4, liquordoor PB5
	DDRC = 0x0F;	//Set PC7...PC3 to input, PC2...PC0 to output [0000 1111]
	PORTC = 0xF0;	//Init port C to 1s							  [1111 0000]
	DDRD = 0xFF;	//Set Port D to output
	PORTD = 0x00;	//Init Port D to 0s
	
	nokia_lcd_init();
	
	HX711_init(64);
	HX711_set_offset(8404767);
	HX711_set_scale(-160.6693354);
	
	HX711_1_init(128);
	HX711_1_set_offset(8409384);
	HX711_1_set_scale(-946.4825995);

	
    //Start Tasks  
    MainMenuPulse(1);
	AddUserPulse(1);
	RemoveUsersPulse(1);
	AddDrinkPulse(1);
	ReplaceDrinkPulse(1);
	UserVerifyPulse(1);
	GetDrinkPulse(1);
	GetKeyPulse(1);
	AutoLockLiquorDoorPulse(1);
	AutoLockKeyDoorPulse(1);
	TimeElapsedPulse(1);
	
    //RunSchedular 
    vTaskStartScheduler(); 
	return 0; 
}