/* This file is reuse of the lcd library for ssd1306/sh1106 oled-displayby Michael Koehler and distributed under the same license */ 

/*-------------------------------------------------------------------------
# 
#  
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
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
#include "i2c.h"
#include "lcd.h"
#include <avr/interrupt.h>
#include "./fonts/Indie Flower Regular_digits_40.h"
#include "./fonts/ascii_6x8.h"

#define CHAR_HEIGHT CHAR_HEIGHT_40
#define CHAR_WIDTH CHAR_WIDTH_40-2

void lcd_init(uint8_t dispAttr){
    if(LCD_INIT_I2C == YES) i2c_init();
    uint8_t commandSequence[sizeof(init_sequence)+1];
    for (uint8_t i = 0; i < sizeof (init_sequence); i++) {
        commandSequence[i] = (pgm_read_byte(&init_sequence[i]));
    }
    commandSequence[sizeof(init_sequence)]=(dispAttr);
    lcd_command(commandSequence, sizeof(commandSequence));
    lcd_clrscr();
}

void lcd_home(void){
	lcd_gotoxy(0, 0);
}

void lcd_command(uint8_t cmd[], uint8_t size) {
    lcd_send_i2c_start();
    lcd_send_i2c_byte(0x00);	// 0x00 for command, 0x40 for data
    for (uint8_t i=0; i<size; i++) {
        lcd_send_i2c_byte(cmd[i]);
    }
    lcd_send_i2c_stop();
}
void lcd_data(uint8_t data[], uint16_t size) {
    lcd_send_i2c_start();
    lcd_send_i2c_byte(0x40);	// 0x00 for command, 0x40 for data
    for (uint8_t i; i<size; i++) {
        lcd_send_i2c_byte(data[i]);
    }
    lcd_send_i2c_stop();
}
void lcd_gotoxy(uint8_t x, uint8_t y){
    if( x > (DISPLAY_WIDTH) || y > (DISPLAY_HEIGHT/8-1)) return;// out of display
    //x = x * 8;					// one char: 6 pixel width
#if defined SSD1306
    uint8_t commandSequence[] = {0xb0+y, 0x21, x, 0x7f};
#elif defined SH1106
    uint8_t commandSequence[] = {0xb0+y, 0x21, 0x00+((2+x) & (0x0f)), 0x10+( ((2+x) & (0xf0)) >> 4 ), 0x7f};
#endif
    lcd_command(commandSequence, sizeof(commandSequence));
}

void lcd_clrscr(void){
    lcd_home();
    for (uint16_t i = 0; i < DISPLAYSIZE; i++) {
        lcd_data(0x01, 1);
    }

	lcd_home();
}

void lcd_putc(char c){
	if((c > 127 || 
	   c < 32) &&
	   (c != 'ü' &&
		c != 'ö' &&
		c != '°' &&
		c != 'ä' &&
		c != 'ß' &&
		c != 'Ü' &&
		c != 'Ö' &&
		c != 'Ä' ) ) return;
    
    lcd_send_i2c_start();
    lcd_send_i2c_byte(0x40);	// 0x00 for command, 0x40 for data
    	switch (c) {
		case 'ü':
			c = 95; // ü
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case 'ö':
			c = 99; // ö
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case '°':
			c = 101; // °
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case 'ä':
			c = 97; // ä
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case 'ß':
			c = 102; // ß
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case 'Ü':
			c = 96; // Ü
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case 'Ö':
			c = 100; // Ö
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		case 'Ä':
			c = 98; // Ä
			for (uint8_t i = 0; i < 6; i++)
			{
				lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
			}
			break;
		default:
                c -= 32;
                if( c < 127-32 ) {
                    for (uint8_t i = 0; i < 6; i++)
                    {
                        lcd_send_i2c_byte(pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]));	// print font to ram, print 6 columns
                    }
                    //return;
                }
			break;
	}
    lcd_send_i2c_stop();
}

void lcd_puts(const char* s){
    while (*s) {
        lcd_putc(*s++);
    }
}
void lcd_puts_p(const char* progmem_s){
    register uint8_t c;
    while ((c = pgm_read_byte(progmem_s++))) {
        lcd_putc(c);
    }
}

void lcd_invert(uint8_t invert){
    lcd_send_i2c_start();
    uint8_t commandSequence[1];
    if (invert == YES) {
        commandSequence[0] = 0xA7;
    } else {
        commandSequence[0] = 0xA7;
    }
    lcd_command(commandSequence, 1);
}

void lcd_set_contrast(uint8_t contrast){
    uint8_t commandSequence[2] = {0x81, contrast};
    lcd_command(commandSequence, sizeof(commandSequence));
}

void lcd_put_bigc(const char* addr, uint8_t column, uint8_t char_width, uint8_t general_width, uint8_t char_height){
	uint8_t x_offset = ((general_width - char_width)>>1); 

	for (uint8_t i = 0; i < (char_height >> 3); i++) {
		lcd_gotoxy(column, 1+i); 
		lcd_send_i2c_start();
    	lcd_send_i2c_byte(0x40);
	
		for (uint8_t j = 0; j < general_width; j++)
			if(j < x_offset) {
				lcd_send_i2c_byte(0x00);
				}
			else {
				if(j < (x_offset + char_width)) {
		   			lcd_send_i2c_byte(pgm_read_byte(&addr[j-x_offset+char_width*i])); 
					}
				else {
					lcd_send_i2c_byte(0x00);
					}
				}		

		lcd_send_i2c_stop();		
    }
}


void lcd_clear_bigc(uint8_t column, uint8_t char_height){
	for (uint8_t i = 0; i < (char_height >> 3) ; i++) {
		lcd_gotoxy(column, 1+i); 
		lcd_send_i2c_start();
    	lcd_send_i2c_byte(0x40);
		for (uint8_t j = 0; j < 18; j++)
			{
   			lcd_send_i2c_byte(0x00); 
			}
		lcd_send_i2c_stop();
		
    }
}


void lcd_update_6(uint8_t c5,uint8_t c4,uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0){
	lcd_put_bigc (bitmap_colon, 75, char_width[10], CHAR_WIDTH-6, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_colon, 32, char_width[10], CHAR_WIDTH-6, CHAR_HEIGHT);
	if(c5 == 0)
		lcd_clear_bigc(0, CHAR_HEIGHT); 
	else
		lcd_put_bigc (bitmap_addr[c5], 0, char_width[c5], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[c4], 18, char_width[c4], CHAR_WIDTH, CHAR_HEIGHT);	
	
	lcd_put_bigc (bitmap_addr[c3], 42, char_width[c3], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[c2], 60, char_width[c2], CHAR_WIDTH, CHAR_HEIGHT);
	
	lcd_put_bigc (bitmap_addr[c1], 85, char_width[c1], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[c0], 103, char_width[c0], CHAR_WIDTH, CHAR_HEIGHT);

}


void lcd_update_4(uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0, uint8_t ampm){
	/*if(c3 == 0)
		lcd_clear_bigc(0); 
	else*/
	// Check if mode was set to AM/PM
        // Substract 12 hours if time is in afternoon
         
	if(ampm) {
		lcd_gotoxy(100,6); 
		if(((c3 << 4) + c2) > 0x12) {
		  if((c2 == 0) | (c2 == 1)){
		    c3 -= 2; 
		    c2 += 8;  
		  }
		  else {
			c3 -= 1;
			c2 -= 2;
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
	// Write 5 characters to display
	lcd_put_bigc(bitmap_colon, 48, char_width[10], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[c3], 0, char_width[c3], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[c2], 24, char_width[c2], CHAR_WIDTH, CHAR_HEIGHT);	
	lcd_put_bigc (bitmap_addr[c1], 72, char_width[c1], CHAR_WIDTH, CHAR_HEIGHT);
	lcd_put_bigc (bitmap_addr[c0], 96, char_width[c0], CHAR_WIDTH, CHAR_HEIGHT);

}
