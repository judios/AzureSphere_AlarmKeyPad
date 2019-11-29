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

#include "Event_Handler.h"


char const F2_STATUS_EVENT = 0xF2;
char const F6_ACK_EVENT = 0xF6;
char const F7_DISPLAY_EVENT = 0xF7;
char const F9E_UNK_EVENT = 0xF9;

/*
 * Reads N Chars from the serial port
 * @param int ct number of characters to read
 * @param char *bufferHex destination buffer
 */
void read_chars(int ct, char *bufferHex ) {

  int x=1;
	
  while (x < ct) {
    if (alarmKeyPad_Available()) {
      bufferHex[ x ] = alarmKeyPad_Read();
      x++;
    }
  }
  bufferHex[x] = '\0';    
  
}

/*
 * Reads dynamic chars, this function expects that the first byte
 * specifies the lenght of the message
 * @param char *bufferHex destination buffer
 * @return int numbers of characters read, -1 if no enought space to store read data
 */
int read_chars_dyn(char* bufferHex) {

	short x = 2, limit = 0;

	// wait for the rest of the data
	while (alarmKeyPad_Available() == 0);

	bufferHex[1] = alarmKeyPad_Read();

	limit = bufferHex[1] + 2;
	while (x < limit) {
		if (alarmKeyPad_Available() > 0) {
			bufferHex[x] = alarmKeyPad_Read();
			x++;
		}
	}
	return x;
}


