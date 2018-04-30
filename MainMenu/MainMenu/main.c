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

////////// Add a user globals //////////
char user_name[14] = "_";
unsigned char name_length = 0;

unsigned char last_key = '\0';
unsigned char new_key = '\0';
unsigned char name_finished = 0;
////////// End add a user globals //////////
	
typedef struct{
	unsigned char name[14];
	unsigned int feet;
	unsigned int inches;
	unsigned int weight;
	unsigned char gender;
	unsigned char password[9];

} User;

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
	unsigned char tens = 0;
	unsigned char ones = 0;
	unsigned long temp_timer = 0;
	while((pushed_key_ANP = GetKeypadKey()) == '\0'){
		if(auto_timer == 40){
			auto_timer = 0;
			return '\0';
		}
		temp_timer = auto_timer;
		
		tens = temp_timer / 10;
		ones = temp_timer % 10;
		
		nokia_lcd_clear();
		nokia_lcd_set_cursor(0,30);
		nokia_lcd_write_char(tens + '0',1);
		nokia_lcd_set_cursor(8,30);
		nokia_lcd_write_char(ones + '0',1);
		nokia_lcd_render();
		
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
				mainmenu_state = add_drink;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Add Drink*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			
			else if(drink_menu_selection == '2'){
				mainmenu_state = replace_drink;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Replce Drnk*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			
			else{
				mainmenu_state = drink_options;
			}
			
			break;
			
		case add_drink:
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
				mainmenu_state = add_drink;
			}
			
			break;
			
		case replace_drink:
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
				mainmenu_state = replace_drink;
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
				nokia_lcd_clear();
				nokia_lcd_write_string("*Remove Usr*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
			}
			
			else if(user_menu_selection == '3'){
				mainmenu_state = reset_password;
				nokia_lcd_clear();
				nokia_lcd_write_string("*Rst Pass*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
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
				mainmenu_state = remove_user;
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
 
enum AddUser {userinit, name, weight, height, gender, password, confirm,} adduser_state;

void AddUser_Init(){
	adduser_state = userinit;
} 

void AddUser_Tick(){
	// Actions
	switch (adduser_state){
		case userinit:
			break;
			
		case name:
			while(!name_finished){	
				new_key = AlphaNumPad();
				while(new_key != '#'){
					if(new_key == '*'){
						if(name_length > 0){
							--name_length;
							user_name[name_length] = '_';
							user_name[name_length + 1] = '\0';
							adduser_name_display(user_name);
							last_key = '\0';
						}
					}
				
					else if(new_key != '\0'){
						last_key = new_key;
						user_name[name_length] = new_key;
						user_name[name_length + 1] = '\0';
						adduser_name_display(user_name);
					}
				
				
					else{
						if(last_key != '\0'){
							if(name_length < 10){
								last_key = '\0';
								user_name[name_length + 1] = '_';
								user_name[name_length + 2] = '\0';
								++name_length;
								adduser_name_display(user_name);
							}
						}
					}
				}
				name_finished = 1;
				
				new_key = '#';
			}
				
			
			break;
		/*
		case weight:
			break;
		
		case height:
			break;
		
		case gender:
			break;
		
		case password:
			break;
		
		case confirm:
			break;
		*/	
		default:
			break;
	}
	
	// Transitions
	switch (adduser_state){
		case userinit:
			if(adding_user_flag){
				user_name[0] = '_';
				name_length = 0;

				last_key = '\0';
				new_key = '\0';
				name_finished = 0;
				
				adduser_state = name;
			}
			
			break;
		
		case name:
			if(new_key == '#'){
				adding_user_flag = 0;
				adduser_state = userinit;
			}
			
			break;
		/*
		case weight:
			if(GetKeypadKey() == '#'){
				while(GetKeypadKey() == '#') {}
				
				adduser_state = gender;
			}
			
			break;
				
		case gender:
			if(GetKeypadKey() == '#'){
				while(GetKeypadKey() == '#') {}
				
				adduser_state = password;
			}
			
			break;
		
		case password:
			if(GetKeypadKey() == '#'){
				while(GetKeypadKey() == '#') {}
				
				adduser_state = confirm;
			}
			
			break;
		
		case confirm:
			if(GetKeypadKey() == '#'){
				while(GetKeypadKey() == '#') {}
				
				adduser_state = userinit;
				adding_user_flag = 0;
			}
			break;
		*/
		default:
			adduser_state = userinit;
			break;
	}
}

void AddUserTask(){
	AddUser_Init();
	for(;;)
	{
		AddUser_Tick();
		vTaskDelay(1);
	}
}

void AddUserPulse(unsigned portBASE_TYPE Priority){
	xTaskCreate(AddUserTask, (signed portCHAR *)"AddUserTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
 
 ////////// Adding A User State Machine //////////
 
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
	nokia_lcd_init();

    //Start Tasks  
    MainMenuPulse(1);
	AddUserPulse(1);
    //RunSchedular 
    vTaskStartScheduler(); 
	return 0; 
}