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
#ifndef F2_MESSAGE_HANDLER_H
#define F2_MESSAGE_HANDLER_H

#include "Event_Handler.h"
extern int const F2_BYTE1_STATE_DISARMED;
extern int const F2_BYTE1_STATE_ARMED;
extern int const F2_BYTE1_STATE_CHIME;
extern int const F2_BYTE1_STATE_BUSY;

extern int const F2_BYTE2_ARMED_MODE_STAY;
extern int const F2_BYTE2_ARMED_MODE_AWAY;

extern int const F2_BYTE4_FAULT_NORMAL;
extern int const F2_BYTE4_FAULT_COUNTING_EXIT;
extern int const F2_BYTE4_FAULT_COUNTING_ENTER;
extern int const F2_BYTE4_FAULT_ALARM;
extern int const F2_BYTE4_FAULT_BYPASSED;
extern int const F2_BYTE4_FAULT_PANIC;

// F2 Message
#define F2_MAX_MESSAGE_LEN 128

void statusHandler_Init();
int statusHandler_handle_event(char et);
void statusHandler_reset();
void statusHandler_set_read_size(int readSizePa);
int statusHandler_get_read_size();
int statusHandler_parse();
int statusHandler_get_counter();
int statusHandler_get_partition();
int statusHandler_get_state();
void statusHandler_get_state_str(char*);
int statusHandler_get_armed_mode();
void statusHandler_get_armed_mode_str(char*);
int statusHandler_get_by_pass();
int statusHandler_get_fault();
void statusHandler_get_fault_str(char*);
int statusHandler_get_count_down();
void statusHandler_to_string(char*);
void statusHandler_debug_to_string(char*);

#endif
