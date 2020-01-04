/*-------------------------------------------------------------------------

  # (c) 2019 JD Morise
  # 
  # Simple Clock programm with Microchip ATmega8A and SSD1306 OLED Display
  # Alarm


  #-------------------------------------------------------------------------
  # (C) 2019 JD Morise, jdmorise@yahoo.com
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

//#########################################################

//#########################################################
// 
// Version 0.3.1
// Bugfix #00005: Calibration Value updated for timer 
//
//#########################################################


#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "i2c.h"
#include "clock.h"

#define timer_reg_div ((1 << CS12) | (1 << CS10)) // Timer with divide ratio of 8

/* Variables for time control */

// Duration of Alarm
#define alarm_dur_one  0
#define alarm_dur_ten 1 
uint8_t hour_ten,hour_one,min_ten,min_one,sec_ten,sec_one;
uint8_t alarm_hour_ten = 2; 
uint8_t alarm_hour_one = 3; 
uint8_t alarm_min_ten = 0; 
uint8_t alarm_min_one = 1; 
//uint8_t alarm_stop
uint8_t alarm_trig_ten = 0;
uint8_t alarm_stop_one = alarm_dur_one; 
//uint8_t alarm_stop_min = alarm_min;  

int16_t msec; // The variable contains is 1/16 sec, not 1 millisec

static uint8_t update_lcd = 0;

int16_t timer_cal_val = 10000; 
int8_t timer_cal_div = 1;
uint16_t timer_reg_val = 391;

/* EEPROM Variables */ 
uint8_t EEMEM EE_timer_cal_div = (1 << CS11);
uint16_t EEMEM EE_timer_cal_val = 49400; // Calibrated Timer Value (division of 8) 
uint16_t EEMEM EE_timer_reg_val = 390; // Calibrated Regular Timer Value  (division of 1024) 

// correct initial values of TIME
uint8_t EEMEM EE_init_hour_ten = 2; 
uint8_t EEMEM EE_init_hour_one = 0; 
uint8_t EEMEM EE_init_min_ten = 1; 
uint8_t EEMEM EE_init_min_one = 4;
uint8_t EEMEM EE_init_sec_ten = 0; 
uint8_t EEMEM EE_init_sec_one = 0;

/* Status Definitions */ 
uint8_t state = 0; 
uint8_t alarm = 0;
uint8_t alarm_enabled = 1; 
uint8_t alarm_rings = 0;  
uint8_t ampm = 1; 
uint8_t keys = 0; 
uint8_t keysx = 0; 
uint8_t key_changed = 0; 

/* Timer Interrupt */
ISR(TIMER1_COMPA_vect){

  msec++;
	
  if (msec == 9) {
		
    OCR1A = timer_cal_val;  
    TCCR1B = ((1 << WGM12) | timer_cal_div);
    TCNT1 = 0; 
  }	

  if (msec == 10){	
    OCR1A = timer_reg_val;
    TCCR1B = ((1 << WGM12) | timer_reg_div);
    TCNT1 = 0;
	
    sec_one++; 
    msec = 0; 
		
    if (sec_one == 10){
      sec_ten++; 
      sec_one=0;
    }		
    if (sec_ten == 6) {min_one++; sec_ten=0; update_lcd = 1;}
    if (min_one == 10) {min_ten++; min_one=0;}
    if (min_ten == 6) {hour_one++; min_ten=0;}
    if (hour_one == 10) {hour_ten++; hour_one=0;}
    if ((hour_ten == 2) & (hour_one == 4)) {
      hour_ten = 0; 
      hour_one = 0;
    }
		
    if(alarm_rings == 1) {
      if(sec_one == 1) {
	alarm_start();
	alarm_trig_ten = sec_ten; 
	if(alarm_trig_ten > 5) alarm_trig_ten = 0; 
      }
      if ((sec_ten == alarm_trig_ten + 1) & (sec_one == 0)) {
	alarm_rings = 0; 
	alarm_stop(); 
      }
			
    }
    if(alarm_enabled){
      if((hour_ten == alarm_hour_ten) && (hour_one == alarm_hour_one) && (min_ten == alarm_min_ten) && (min_one == alarm_min_one) && (sec_ten == 0) && (sec_one == 0)) {
			
	alarm_rings = 1;  
	alarm_trig_ten = 0; 
		
      }
    }
		
  }

  // Check if key was pressed  
  keys = (PIND & 7); 
  if (keys != keysx) key_changed = 1; 
  else key_changed = 0; 
  keysx = keys; 

  if (key_changed) {
    if ((keys & 1) == 0) key1_pressed();
    if ((keys & 2) == 0) key2_pressed(); 			
    clock_set_statusl(state, alarm_enabled, ampm);	
  }
		
  if(update_lcd == 1) {
    if((state == STATE_SET_ALARM_HOUR) | (state == STATE_SET_ALARM_MIN)) {
      lcd_update_4(alarm_hour_ten, alarm_hour_one, alarm_min_ten, alarm_min_one, ampm); 
    }
    else {
      lcd_update_4(hour_ten, hour_one, min_ten, min_one, ampm);
    }
    update_lcd = 0; 
  }
}


// Main function
//  

int main (void)
{
	
  /* Init PortD as input */ 
  DDRD = 0x00; 
  PORTD = ((1 << PD2) | (1 << PD1) | (1<<PD0));  

  /* PortB as output */ 
  DDRB = 0xff; 
  PORTB = 0xff & (0 << PB3); 
  /* Init OLED Display */
	
  lcd_init(LCD_DISP_ON);
  //lcd_clrscr();
        
  /* put string to display (line 7) with linefeed */
  lcd_clear_screen();
  lcd_set_contrast(196);
	
  /* Read Timer Calibration Values from EEPROM */
  timer_cal_div = eeprom_read_byte(&EE_timer_cal_div); 
  timer_cal_val = eeprom_read_word(&EE_timer_cal_val);
  timer_reg_val = eeprom_read_word(&EE_timer_reg_val);

  /* set Timer 1 for Clock Control */
  TCCR1B = ((1 << WGM12) | timer_reg_div);
  OCR1A = timer_reg_val;

  /* Timer Interrupt Control*/
  TIMSK = (1 << OCIE1A); 

  /* Global Interrupt Enable */ 
  sei();

  /* Initialize time variables */ 
  sec_one = eeprom_read_byte(&EE_init_sec_one);
  sec_ten = eeprom_read_byte(&EE_init_sec_ten); 

  min_one = eeprom_read_byte(&EE_init_min_one);
  min_ten = eeprom_read_byte(&EE_init_min_ten);

  hour_one = eeprom_read_byte(&EE_init_hour_one); 
  hour_ten = eeprom_read_byte(&EE_init_hour_ten);


  /* Update LCD Display */ 

  //lcd_update_4(min_ten, min_one, sec_ten, sec_one);  
	
  lcd_update_4(hour_ten, hour_one, min_ten, min_one, ampm); 
  clock_set_statusl(state, alarm_enabled, ampm);

  while (1) {}
  return 0; 
	
}

/* Function to change state if key1 is pressed */
void key1_pressed(void){
  switch(state) {
  case STATE_CLOCK:
    state = STATE_SET_HOUR; 
    break; 

  case STATE_SET_HOUR:				
    state = STATE_SET_MIN;
    break;	
							
  case STATE_SET_MIN:
    update_lcd = 1; 	
    state = STATE_SET_ALARM_HOUR;
    break; 

  case STATE_SET_ALARM_HOUR:
    state = STATE_SET_ALARM_MIN;
    break;	

  case STATE_SET_ALARM_MIN:
    update_lcd = 1;
    state = STATE_SET_ALARM;
    break; 
  case STATE_SET_ALARM: 
    state = STATE_SET_AMPM; 
    break; 
				
  case STATE_SET_AMPM: 
    state = STATE_CLOCK; 
    break; 
  }
}

/* Function to execute changes for key2 */ 
void key2_pressed(void) {
  switch(state) {
  case STATE_CLOCK: 
    alarm_rings = 1; 
    break; 

  case STATE_SET_HOUR:
    update_lcd = 1;
    hour_one += 1; 
    if (hour_one > 9) {
      hour_one = 0; 
      hour_ten +=1;
      if (hour_ten > 2) hour_ten = 0;
    }
    if(hour_ten == 2 && hour_one == 4) { 
      hour_one = 0; 
      hour_ten = 0; 
    }  
    break; 
				
  case STATE_SET_MIN:
    update_lcd = 1;
    min_one += 1; 
    if(min_one > 9) {
      min_ten += 1;
      min_one = 0; 
      if(min_ten > 5) min_ten = 0; 
    }
    break; 
					 
  case STATE_SET_ALARM_HOUR:
    update_lcd = 1;
    alarm_hour_one += 1; 
    if (alarm_hour_one > 9) {
      alarm_hour_one = 0; 
      alarm_hour_ten +=1;
      if (alarm_hour_ten > 2) alarm_hour_ten = 0;
    }
    if(hour_ten == 2 && hour_one == 4) { 
      hour_one = 0; 
      hour_ten = 0; 
    }  
    break; 

  case STATE_SET_ALARM_MIN:
    update_lcd = 1;
    alarm_min_one += 1; 
    if(alarm_min_one > 9) {
      alarm_min_ten += 1;
      alarm_min_one = 0; 
      if(alarm_min_ten > 5) alarm_min_ten = 0; 
    }
    break; 

  case STATE_SET_ALARM: 
    alarm_enabled = (~alarm_enabled) & 0x1; 	 
    break; 

  case STATE_SET_AMPM: 
    ampm = (~ampm) & 0x1;
    break; 
  }		
} 

