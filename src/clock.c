
//#########################################################
// (c) 2019 JD Morise
// 
// clock library with Microchip ATmega8A and SSD1306 OLED Display
// 

/*-------------------------------------------------------------------------
# 
# (C) 2019, jdmorise@yahoo.com
#  
# 	 This program is free software: you can redistribute it and/or modify
# 	 it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#
#-------------------------------------------------------------------------
*/

//#########################################################

//#########################################################
// 
// Version 0.2.1
// Minor bug fix for display of hours > 20 
//
//#########################################################

#include "lcd.h"
#include "clock.h"
#include "./fonts/Indie Flower Regular_digits_32.h"
#include "./fonts/ascii_6x8.h"
#define CHAR_HEIGHT CHAR_HEIGHT_32
#define CHAR_WIDTH CHAR_WIDTH_32-2

/* Update OLED with 4 characters and DOT */
/* Unused Function */ 
void lcd_update_time(uint8_t hour, uint8_t min, uint8_t ampm){
	/*if(c3 == 0)
		lcd_clear_bigc(0); 
	else*/
	
	if(ampm) {
		lcd_gotoxy(100,6); 
		if(hour > 0x12) {
			if((hour == 0x20)){
			  hour -= 0x18; 
			}
			else {
			  hour -= 0x12; 
			}

			lcd_puts("PM");  
		}
		else {
		  lcd_puts("AM"); 
		}
	}
	else {
	  lcd_puts("  "); 
	}	
	lcd_put_bigc(bitmap_colon, 48, char_width[10], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[hour>>4], 0, char_width[hour>>4], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[hour&0x0f], 24, char_width[hour&0x0f], CHAR_WIDTH, CHAR_HEIGHT);	
	lcd_put_bigc (bitmap_addr[min>>4], 72, char_width[min>>4], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[min&0x0f], 96, char_width[min&0xf], CHAR_WIDTH, CHAR_HEIGHT);

}

/* Clear Screen */ 
void lcd_clear_screen() {
	for(uint8_t i = 0; i<8; i++) {
		lcd_gotoxy(0, i); 
		lcd_puts("                      "); 
	}

}

/* Alarm */ 
void alarm_start(void){
	PORTB = (1 << PB0); 
}


/* Stop Alarm function */ 
void alarm_stop(void){
	PORTB = (0 << PB0); 
}

/* Function to set Statusline of LCD Display */ 
void clock_set_statusl(uint8_t state, uint8_t alarm_enabled, uint8_t ampm){ 

switch (state) {
	case STATE_SET_HOUR: 
		lcd_gotoxy(0,6); 
		lcd_puts("    X                  ");
		lcd_gotoxy(0,7); 
		lcd_puts("Set Clock              ");
	break; 

	case STATE_SET_MIN: 
		lcd_gotoxy(0,6);
		lcd_puts("                X     ");	 
		lcd_gotoxy(0,7); 
		lcd_puts("Set Clock             ");
	break; 

	case STATE_SET_ALARM_HOUR: 
		lcd_gotoxy(0,6); 
		lcd_puts("    X                  ");
		lcd_gotoxy(0,7); 
		lcd_puts("Set Alarm              ");
	break; 

	case STATE_SET_ALARM_MIN:
		lcd_gotoxy(0,6);
		lcd_puts("                X     ");	 
		lcd_gotoxy(0,7); 
		lcd_puts("Set Alarm             ");
	break; 

	case STATE_CLOCK:
		lcd_gotoxy(0,6);
		lcd_puts("                      ");	 	
		lcd_gotoxy(0,7); 
		lcd_puts("Yet Another AVR Clock "); 
	break; 

	case STATE_SET_AMPM:
		lcd_gotoxy(0,6);
		lcd_puts("                      ");	 			
		lcd_gotoxy(0,7); 
		if(ampm == 0) lcd_puts("Mode: Normal            "); 
		else lcd_puts("Mode: AMPM               "); 
	break; 

	case STATE_SET_ALARM:
		lcd_gotoxy(0,6);
		lcd_puts("                      ");	 			
		lcd_gotoxy(0,7); 
		if(alarm_enabled == 0) lcd_puts("Alarm: Aus            "); 						       
		else lcd_puts("Alarm: Ein            ");
	break; 
	}
}


