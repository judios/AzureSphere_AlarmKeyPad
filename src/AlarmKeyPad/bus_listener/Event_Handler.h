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

#pragma once

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

extern char const F2_STATUS_EVENT;
extern char const F6_ACK_EVENT;
extern char const F7_DISPLAY_EVENT;
extern char const F9E_UNK_EVENT;

#include "..\AlarmKeyPad.h"

void read_chars(int ct, char *bufferHex );
int read_chars_dyn(char *bufferHex);


#endif
