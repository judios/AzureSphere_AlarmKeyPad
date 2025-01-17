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



#include "Msg9e_Handler.h"

char buffer[F9E_MESSAGE_LEN + 4];

/* 
 * Constructor 
 */
void msg9eHandler_Init() {
    
}

/* 
 * Reset the buffer to zero
 */
void msg9eHandler_reset() {
    memset(buffer, 0x00, sizeof(buffer));
}

/* 
 * Reads from the serial port F9 message
 */
int msg9eHandler_handle_event(char et) {
	msg9eHandler_reset();
    buffer[0] = (unsigned char)(et & 0xff);
    read_chars( F9E_MESSAGE_LEN, buffer );
    return 1;
}


/* 
 * String representation of the message
 */
void msg9eHandler_to_string(char *intBuffer) {
    memset(intBuffer, 0x00, sizeof(intBuffer));
    sprintf( intBuffer, "F9:{" ); 
    char auxBuffer[8];
    
    for ( int i = 0 ; i< 7; i++ ) {        
        sprintf( auxBuffer, " %02x", 0xff & buffer[i] ); 
        strcat(intBuffer, auxBuffer );
    }
    strcat(intBuffer, "}" );
   
}

void msg9eHandler_debug_to_string(char *intBuffer) {
  memset(intBuffer, 0x00, sizeof(intBuffer));
  memcpy(intBuffer, buffer, F9E_MESSAGE_LEN);
}


