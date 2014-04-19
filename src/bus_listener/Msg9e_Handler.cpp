/*
    Copyright 2013 Jose Castellanos Molina
    
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


#include "Arduino.h"
#include "Msg9e_Handler.h"

/* 
 * Constructor 
 */
Msg9e_Handler::Msg9e_Handler() : Event_Handler() {
    
}

/* 
 * Reset the buffer to zero
 */
void Msg9e_Handler::reset() {
    memset(buffer, 0x00, sizeof(buffer));
}

/* 
 * Reads from the serial port 9E message
 */
int Msg9e_Handler::handle_event(char et) {
    reset();    
    buffer[0] = (unsigned char)(et & 0x00ff);
    read_chars( F9E_MESSAGE_LEN, buffer );
    return 1;
}


/* 
 * String representation of the message
 */
char * Msg9e_Handler::to_string() {
    char auxBuffer[32];
    memset(auxBuffer, 0x00, sizeof(auxBuffer));

    //sprintf( auxBuffer, "!M9E:" ); 
    //for ( int i = 1 ; i< F9E_MESSAGE_LEN+1; i++ ) {
    //    sprintf( auxBuffer, "[%02x]", buffer[i] ); 
    //}
       
    return auxBuffer;
}
