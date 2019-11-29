/*
	Copyright 2013 Jose Castellanos Molina
    
	Modified in 2019 for use with AlarmKeyPad project by Julian Diaz

	This file is part of homesecurity.

	homesecurity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	homesecurity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with homesecurity.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef F7_MESSAGE_HANDLER_H
#define F7_MESSAGE_HANDLER_H

#include "Event_Handler.h"
#include <stdbool.h>

void displayHandler_Init();
int displayHandler_handle_event(char et);
void displayHandler_reset();
int displayHandler_get_addr1();
int displayHandler_get_addr2();
int displayHandler_get_addr3();
int displayHandler_get_addr4();
int displayHandler_get_zone_field();
int displayHandler_get_data1();
int displayHandler_get_beeps();
int displayHandler_get_data2();
bool displayHandler_get_armed_stay();
bool displayHandler_get_ready();
int displayHandler_get_data3();
bool displayHandler_get_chime_mode();
bool displayHandler_get_ac_power();
bool displayHandler_get_armed_away();
bool displayHandler_get_programming_mode();
bool displayHandler_is_prompt_displaying();
bool displayHandler_get_alarm_ocurred();
bool displayHandler_get_alarm_bell();
bool displayHandler_get_entry_delay_off();
bool displayHandler_get_perimeter_only();
int displayHandler_get_prompt_position();
void displayHandler_get_display_message(char*);
int displayHandler_get_check_sum();
void displayHandler_to_string(char*);
void displayHandler_debug_to_string(char*);


#endif
