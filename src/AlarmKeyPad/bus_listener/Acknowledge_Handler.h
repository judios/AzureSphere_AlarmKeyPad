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

#ifndef F6_MESSAGE_HANDLER_H
#define F6_MESSAGE_HANDLER_H

#include "Event_Handler.h"

#define F6_MESSAGE_LEN 3

void acknowledgeHandler_Init();
int acknowledgeHandler_handle_event (char et);
void acknowledgeHandler_reset();
int acknowledgeHandler_get_ack_address();
int acknowledgeHandler_get_seq_number();
void acknowledgeHandler_to_string(char *);
void acknowledgeHandler_debug_to_string(char *);

#endif
