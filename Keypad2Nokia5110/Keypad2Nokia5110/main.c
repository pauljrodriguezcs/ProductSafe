/*
 * Keypad2Nokia5110.c
 *
 * Created: 4/13/2018 1:10:49 AM
 * Author : Paul
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "keypad.h"
#include "nokia5110.h"

unsigned char previous_key = '\0';
unsigned char tmp;
unsigned char pushed_key;
unsigned int num_times_key_pushed = 0;
unsigned long timer = 0;

void outputchar(unsigned char c){
	nokia_lcd_clear();
	nokia_lcd_write_char(c,1);
	nokia_lcd_render();
}

void outputstring(const char* str){
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(str,1);
	nokia_lcd_render();
}

unsigned char AlphaNumPad(){
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
		
	while((pushed_key = GetKeypadKey()) == '\0'){
		if(timer == 40){
			timer = 0;
			return '\0';
		}
	
		++timer;
	}
	
	while((tmp = GetKeypadKey()) == pushed_key){}
	
	timer = 0;
	
	if(previous_key == pushed_key){
		if(num_times_key_pushed < 3){
			num_times_key_pushed++;
		}
		
		else{
			num_times_key_pushed = 0;
		}
		
		if(pushed_key == '1'){ return one[num_times_key_pushed]; }
		else if(pushed_key == '2'){ return (two[num_times_key_pushed]); }
		else if(pushed_key == '3'){ return (three[num_times_key_pushed]); }
		else if(pushed_key == '4'){ return (four[num_times_key_pushed]); }
		else if(pushed_key == '5'){ return (five[num_times_key_pushed]); }
		else if(pushed_key == '6'){ return (six[num_times_key_pushed]); }
		else if(pushed_key == '7'){ return (seven[num_times_key_pushed]); }
		else if(pushed_key == '8'){ return (eight[num_times_key_pushed]); }
		else if(pushed_key == '9'){ return (nine[num_times_key_pushed]); }
		else if(pushed_key == '0'){return (zero[num_times_key_pushed]); }
		else if(pushed_key == '*'){return (star[num_times_key_pushed]); }
		else if(pushed_key == '#'){return (pound[num_times_key_pushed]); }
			
	}
	
	else{
		previous_key = pushed_key;
		num_times_key_pushed = 0;
		
	}
	
	return previous_key;
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
	nokia_lcd_clear();
		
	char intro[] = "Hello";
	nokia_lcd_write_string(intro,1);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_char('_',1);
	nokia_lcd_render();
	
	char name[14] = "_";
	unsigned char name_length = 0;
	
	unsigned char last_key = '\0';
	unsigned char new_key = '\0';
	unsigned char finished = 0;
	
    
	while (1) 
    {	
		while(!finished){
			new_key = AlphaNumPad();
		
			while(new_key != '#'){
				if(new_key == '*'){
					if(name_length > 0){
						--name_length;
						name[name_length] = '_';
						name[name_length + 1] = '\0';
						outputstring(name);
						last_key = '\0';
					}
				}
				
				else if(new_key != '\0'){
					last_key = new_key;
					name[name_length] = new_key;
					name[name_length + 1] = '\0';
					outputstring(name);
				}
				
		
				else{
					if(last_key != '\0'){
						if(name_length < 10){
							last_key = '\0';
							name[name_length + 1] = '_';
							name[name_length + 2] = '\0';
							++name_length;
							outputstring(name);
						}	
					}
				}
			
				new_key = AlphaNumPad();
			}
			
			name[name_length+1] = '\0';
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string(name,1);
			nokia_lcd_set_cursor(0,30);
			nokia_lcd_write_string("Paul is cool",1);
			nokia_lcd_render();
			
			finished = 1;
		}
		
		
		

		
		
		/*
		while((pushed_key = GetKeypadKey()) == '\0'){}
				
		while((tmp = GetKeypadKey()) == pushed_key){}
		
		if(previous_key == pushed_key){
			if(num_times_key_pushed < 3){
				num_times_key_pushed++;
			}
			
			else{
				num_times_key_pushed = 0;
			}
			
			
			if(pushed_key == '2'){ outputchar(two[num_times_key_pushed]); }
			else if(pushed_key == '3'){ outputchar(three[num_times_key_pushed]); }
			else if(pushed_key == '4'){ outputchar(four[num_times_key_pushed]); }
			else if(pushed_key == '5'){ outputchar(five[num_times_key_pushed]); }
			else if(pushed_key == '6'){ outputchar(six[num_times_key_pushed]); }
			else if(pushed_key == '7'){ outputchar(seven[num_times_key_pushed]); }
			else if(pushed_key == '8'){ outputchar(eight[num_times_key_pushed]); }
			else if(pushed_key == '9'){ outputchar(nine[num_times_key_pushed]); }
		}
		
		else{
			outputchar(pushed_key);
			previous_key = pushed_key;
			num_times_key_pushed = 0;
		}
		*/
		
    }
}

