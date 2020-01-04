
//#########################################################
// (c) 2019 JD Morise
// 
// Library for display handling / clock handling
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
// Version 0.2.0
// Initial Version 
//
//#########################################################


#define STATE_CLOCK 0
#define STATE_SET 1
#define STATE_SET_ALARM_ENABLE
#define STATE_SET_HOUR 10
#define STATE_SET_MIN 11

#define STATE_SET_ALARM_HOUR 20
#define STATE_SET_ALARM_MIN 21

#define STATE_SET_AMPM 2
#define STATE_SET_ALARM 3

extern void lcd_update_time(uint8_t hour, uint8_t min, uint8_t ampm); 
extern void lcd_clear_screen(); 
void clock_set_statusl(uint8_t state, uint8_t alarm_enabled, uint8_t ampm); 
void alarm_start(void);
void alarm_stop(void);
