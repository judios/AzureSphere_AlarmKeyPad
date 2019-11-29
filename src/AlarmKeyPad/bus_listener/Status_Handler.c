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

#include "Status_Handler.h"


int const F2_BYTE1_STATE_DISARMED = 0x01;
int const F2_BYTE1_STATE_ARMED = 0x02;
int const F2_BYTE1_STATE_CHIME = 0x03;
int const F2_BYTE1_STATE_BUSY = 0x04;

int const F2_BYTE2_ARMED_MODE_STAY = 0x01;
int const F2_BYTE2_ARMED_MODE_AWAY = 0x02;


int const F2_BYTE4_FAULT_NORMAL = 0x01;
int const F2_BYTE4_FAULT_COUNTING_EXIT = 0x02;
int const F2_BYTE4_FAULT_COUNTING_ENTER = 0x03;
int const F2_BYTE4_FAULT_ALARM = 0x04;
int const F2_BYTE4_FAULT_BYPASSED = 0x06;
int const F2_BYTE4_FAULT_PANIC = 0x08;


char buffer[F2_MAX_MESSAGE_LEN + 1];
int readSize;
int startPosition;

void statusHandler_Init() {
    
}

void statusHandler_reset() {
    memset(buffer, 0x00, sizeof(buffer));
}

int statusHandler_handle_event(char et) {
	statusHandler_reset();
    buffer[0] = et;
    int readSize = read_chars_dyn( buffer );
    if ( readSize > 0 ) {
		statusHandler_set_read_size(readSize);
        if (statusHandler_parse() > 0 ) {
            return 1;
        }
    }
    return 0;
}

void statusHandler_set_read_size(int readSizePa) {
    readSize = readSizePa;
}

int statusHandler_get_read_size() {
  return readSize;
}

int statusHandler_parse() {
  if ( readSize < 22 ) {
      return -2;
  }
      
  for (int x = 1; x < readSize; x++) {
    if ( buffer[x] == 0xF5 && (x+5) < readSize ) {
      if ( buffer[x+1] == 0xEC) {
        startPosition = x+2;
        return startPosition;
      }
    }
  }
  return -1;
}

int statusHandler_get_counter() {
    return buffer[6];
}

int statusHandler_get_partition() {
    return buffer[7];
}

int statusHandler_get_state() {
  if ( startPosition < readSize ) {
    return buffer[startPosition];
  }

  return -1;
}

int statusHandler_get_armed_mode() {
  if ( (startPosition + 1) < readSize ) {
    return buffer[startPosition+1];
  }

  return -1;
}

int statusHandler_get_by_pass() {
  if ( (startPosition + 2) < readSize ) {
    return buffer[startPosition+2];
  }

  return -1;
}

int statusHandler_get_fault() {
  if ( (startPosition + 3) < readSize ) {
    return buffer[startPosition+3];
  }

  return -1;
}

int statusHandler_get_count_down() {
  if ( (startPosition + 4) < readSize ) {
    return buffer[startPosition+4];
  }

  return -1;
}

void statusHandler_get_state_str(char *res) {
    if (statusHandler_get_state() == F2_BYTE1_STATE_DISARMED ) {
      strcpy(res, "DISAR");
      return;
    } else if (statusHandler_get_state() == F2_BYTE1_STATE_ARMED ) {
      strcpy(res, "ARMED");
      return;
    } else if (statusHandler_get_state() == F2_BYTE1_STATE_CHIME ) {
      strcpy(res, "CHIME");
      return;
    } else if (statusHandler_get_state() == F2_BYTE1_STATE_BUSY ) {
      strcpy(res, "BUSY");
      return;
    } 
    strcpy(res, "");
    return;
}

void statusHandler_get_armed_mode_str(char *res) {
    if (statusHandler_get_armed_mode() == F2_BYTE2_ARMED_MODE_STAY ) {
      strcpy(res, "STAY");
      return;
    } else if (statusHandler_get_armed_mode() == F2_BYTE2_ARMED_MODE_AWAY ) {
      strcpy(res, "AWAY");
      return;
    }  
}

void statusHandler_get_fault_str(char *res) {  
    if (statusHandler_get_fault() == F2_BYTE4_FAULT_NORMAL ) {
      strcpy(res, "NORMAL");
      return;
    } else if (statusHandler_get_fault() == F2_BYTE4_FAULT_COUNTING_EXIT ) {
      sprintf( res, "EXIT %d", statusHandler_get_count_down() );
      return;      
    } else if (statusHandler_get_fault() == F2_BYTE4_FAULT_COUNTING_ENTER ) {
      sprintf( res, "ENTER %d", statusHandler_get_count_down() );
      return;          
    } else if (statusHandler_get_fault() == F2_BYTE4_FAULT_ALARM ) {
      strcpy(res, "ALARM");  
    } else if (statusHandler_get_fault() == F2_BYTE4_FAULT_BYPASSED ) {
      strcpy(res, "BYPAS");
    } else if (statusHandler_get_fault() == F2_BYTE4_FAULT_PANIC ) {
      strcpy(res, "PANIC");
    }  
}

void statusHandler_to_string(char *internalBuffer) {
    char auxBufferS[10];
    char auxBufferM[6];
    char auxBufferF[12];
    memset(internalBuffer, 0x00, sizeof(internalBuffer));
	statusHandler_get_state_str(auxBufferS);
	statusHandler_get_armed_mode_str(auxBufferM);
	statusHandler_get_fault_str(auxBufferF);
    
    sprintf(internalBuffer,"F2:{\"s\":\"%s\",\"m\":\"%s\",\"f\":\"%s\"}\0",auxBufferS,auxBufferM,auxBufferF );
}

void statusHandler_debug_to_string(char *intBuffer) {
  memset(intBuffer, 0x00, sizeof(intBuffer));
  strcat(intBuffer, "F2:{" );
  for ( short i = 0; i < statusHandler_get_read_size(); i++ ) {
      char aux[3];
      sprintf(aux, "%02x", 0xff & buffer[i] );
      strcat(intBuffer, aux );
      strcat(intBuffer, " " );
  }  
  strcat(intBuffer, "}\0" );
  
}
