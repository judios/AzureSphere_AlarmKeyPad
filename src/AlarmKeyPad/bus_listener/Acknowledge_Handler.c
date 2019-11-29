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

#include "Acknowledge_Handler.h"

char buffer[F6_MESSAGE_LEN + 2];

void acknowledgeHandler_Init() {
    
}

/* 
 * Reset the buffer to zero
 */
void acknowledgeHandler_reset() {
    memset(buffer, 0x00, sizeof(buffer));
}

/* 
 * Reads from the serial port f6 message
 */
int acknowledgeHandler_handle_event(char et) {
	acknowledgeHandler_reset();
    buffer[0] = (unsigned char)(et & 0x00ff);
    read_chars( F6_MESSAGE_LEN, buffer );
    return 1;
}

/* 
 * Return the device adress acknowledged
 */
int acknowledgeHandler_get_ack_address() {
    return (int)buffer[1];
}

/* 
 * Return the sequence number
 */
int acknowledgeHandler_get_seq_number() {
    return (int)buffer[2];
}

/* 
 * String representation of the message
 */
void acknowledgeHandler_to_string(char *auxBuffer) {
    sprintf( auxBuffer, "!ACK:[%d][%02x]", acknowledgeHandler_get_ack_address(), 
		acknowledgeHandler_get_seq_number() );

}

void acknowledgeHandler_debug_to_string(char *intBuffer) {
  memset(intBuffer, 0x00, sizeof(intBuffer));
  memcpy(intBuffer, buffer, F6_MESSAGE_LEN);
}
