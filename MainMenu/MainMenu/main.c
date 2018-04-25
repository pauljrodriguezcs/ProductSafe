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

typedef struct{
	unsigned char *name;
	unsigned int feet;
	unsigned int inches;
	unsigned int weight;
	unsigned char gender;
	unsigned char *password;

} User;


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
				nokia_lcd_clear();
				nokia_lcd_write_string("*Add User*",1);
				nokia_lcd_set_cursor(2,10);
				nokia_lcd_write_string("# to return",1);
				nokia_lcd_render();
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
				mainmenu_state = add_user;
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
    //RunSchedular 
    vTaskStartScheduler(); 
	return 0; 
}