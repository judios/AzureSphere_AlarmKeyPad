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
#include "Status_Handler.h"

/* 
 * Constructor 
 */
Status_Handler::Status_Handler() : Event_Handler() {
    
}

void Status_Handler::reset() {
    memset(buffer, 0x00, sizeof(buffer));
}

int Status_Handler::handle_event(char et) {
    reset();
    buffer[0] = (unsigned char)(et & 0x00ff);
    int readSize = read_chars_dyn( buffer );
    if ( readSize > 0 ) {
        set_read_size(readSize);
        if ( parse() > 0 ) {
            return 1;
        }
    }
    return 0;
}

void Status_Handler::set_read_size(int readSizePa) {
    readSize = readSizePa;
}

int Status_Handler::get_read_size() {
  return readSize;
}

int Status_Handler::parse() {
  if ( readSize < 22 ) {
      return -2;
  }
      
  for (int x = 1; x < readSize; x++) {
    if ( buffer[x] == 0xFFF5 && (x+5) < readSize ) {
      if ( buffer[x+1] == 0xFFEC) {
        startPosition = x+2;
        return startPosition;
      }
    }
  }
  return -1;
}

int Status_Handler::get_counter() {
    return buffer[6];
}

int Status_Handler::get_partition() {
    return buffer[7];
}

int Status_Handler::get_state() {
  if ( startPosition < readSize ) {
    return buffer[startPosition];
  }

  return -1;
}

int Status_Handler::get_armed_mode() {
  if ( (startPosition + 1) < readSize ) {
    return buffer[startPosition+1];
  }

  return -1;
}

int Status_Handler::get_by_pass() {
  if ( (startPosition + 2) < readSize ) {
    return buffer[startPosition+2];
  }

  return -1;
}

int Status_Handler::get_fault() {
  if ( (startPosition + 3) < readSize ) {
    return buffer[startPosition+3];
  }

  return -1;
}

int Status_Handler::get_count_down() {
  if ( (startPosition + 4) < readSize ) {
    return buffer[startPosition+4];
  }

  return -1;
}

char * Status_Handler::get_state_str() {
    if ( get_state() == F2_BYTE1_STATE_DISARMED ) {
      return "DISARMED";
    } else if ( get_state() == F2_BYTE1_STATE_ARMED ) {
      return "ARMED";
    } else if ( get_state() == F2_BYTE1_STATE_CHIME ) {
      return "CHIME";
    } else if ( get_state() == F2_BYTE1_STATE_BUSY ) {
      return "BUSY";
    } 
    return "";
}

char * Status_Handler::get_armed_mode_str() {
    if ( get_armed_mode() == F2_BYTE2_ARMED_MODE_STAY ) {
      return "STAY";
    } else if ( get_armed_mode() == F2_BYTE2_ARMED_MODE_AWAY ) {
      return "AWAY";
    }  
}

void Status_Handler::to_string(char *internalBuffer) {
    char auxBuffer[8];
    memset(internalBuffer, 0x00, sizeof(internalBuffer));
    sprintf(internalBuffer,"{\"s\":\"%s\",\"m\":\"%s\",\"f\":\"%s\",\"a\":\"%s\"}", get_state_str(), get_armed_mode_str() );
    
    strcat(internalBuffer, "!AUI:");
    
    sprintf( auxBuffer, "%02x", readSize ); //
    strcat(internalBuffer, auxBuffer);
        
    // Armed startPosition 17
    if ( get_state() == F2_BYTE1_STATE_DISARMED ) {
      strcat(internalBuffer, " DISARMED:");
    } else if ( get_state() == F2_BYTE1_STATE_ARMED ) {
      strcat(internalBuffer, " ARMED:");
    } else if ( get_state() == F2_BYTE1_STATE_CHIME ) {
      strcat(internalBuffer, " CHIME:");
    } else if ( get_state() == F2_BYTE1_STATE_BUSY ) {
      strcat(internalBuffer, " BUSY:");
    }  
   
    // Armed Mode startPosition+1 23
    if ( get_armed_mode() == F2_BYTE2_ARMED_MODE_STAY ) {
      strcat(internalBuffer, " STAY:");
    } else if ( get_armed_mode() == F2_BYTE2_ARMED_MODE_AWAY ) {
      strcat(internalBuffer, " AWAY:");
    }        
    
    // Fault startPosition+3 38
        
    if ( get_fault() == F2_BYTE4_FAULT_NORMAL ) {
      strcat(internalBuffer, " NORMAL:");
    } else if ( get_fault() == F2_BYTE4_FAULT_COUNTING_EXIT ) {
      strcat(internalBuffer, " COUNTING_EXIT ");
      
      if ( get_count_down() != -1 ) {
          sprintf( auxBuffer, "%d", get_count_down() ); //39
          strcat(internalBuffer, auxBuffer);
      }
      
      strcat(internalBuffer, ":"); //40
      
    } else if ( get_fault() == F2_BYTE4_FAULT_COUNTING_ENTER ) {
      strcat(internalBuffer, " COUNTING_ENTER ");
      
      if ( get_count_down() != -1 ) {
          sprintf( auxBuffer, "%d", get_count_down() ); //39
          strcat(internalBuffer, auxBuffer);
      }
      
      strcat(internalBuffer, ":"); //40
      
    
    } else if ( get_fault() == F2_BYTE4_FAULT_ALARM ) {
      strcat(internalBuffer, " ALARM:");  
    } else if ( get_fault() == F2_BYTE4_FAULT_BYPASSED ) {
      strcat(internalBuffer, " BYPASSED:");
    } else if ( get_fault() == F2_BYTE4_FAULT_PANIC ) {
      strcat(internalBuffer, " PANIC:");
    }

}

void Status_Handler::debug_to_string(char *intBuffer) {
  memset(intBuffer, 0x00, sizeof(intBuffer));
  memcpy(intBuffer, buffer, F2_MAX_MESSAGE_LEN);
}
