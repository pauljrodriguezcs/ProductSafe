#ifndef DISPLAYOUT_H
#define DISPLAYOUT_H

#include "nokia5110.h"

void main_menu_display(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Main Menu",1);
	nokia_lcd_set_cursor(2,10);
	nokia_lcd_write_string("1: Liquor", 1);
	nokia_lcd_set_cursor(2, 20);
	nokia_lcd_write_string("2: Keys", 1);
	nokia_lcd_set_cursor(2, 30);
	nokia_lcd_write_string("3: Settings", 1);
	nokia_lcd_render();
	return;
}

void settings_submenu_display(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Settings",1);
	nokia_lcd_set_cursor(2,10);
	nokia_lcd_write_string("1: Drinks",1);
	nokia_lcd_set_cursor(2,20);
	nokia_lcd_write_string("2: Users",1);
	nokia_lcd_set_cursor(2,30);
	//nokia_lcd_write_string("3: System",1);
	//nokia_lcd_set_cursor(2, 40);
	nokia_lcd_write_string("3: Main Menu",1);
	nokia_lcd_render();
	return;
}

void drinks_submenu_display(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Drinks",1);
	nokia_lcd_set_cursor(2,10);
	nokia_lcd_write_string("1: Add Drink",1);
	nokia_lcd_set_cursor(2,20);
	nokia_lcd_write_string("2: Replace",1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("Drink",1);
	nokia_lcd_set_cursor(2,40);
	nokia_lcd_write_string("3: Main Menu",1);
	nokia_lcd_render();
}

void user_submenu_display(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Users",1);
	nokia_lcd_set_cursor(2,10);
	nokia_lcd_write_string("1: Add User",1);
	nokia_lcd_set_cursor(2,20);
	nokia_lcd_write_string("2: Delete User",1);
	nokia_lcd_set_cursor(2,30);
	nokia_lcd_write_string("3: Main Menu",1);
	nokia_lcd_render();
}

void adduser_name_display(const char* str){
	nokia_lcd_clear();
	nokia_lcd_write_string("Enter Name",1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(str,1);
	nokia_lcd_render();
}


#endif //DISPLAYOUT_H