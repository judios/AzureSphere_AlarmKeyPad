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


#include "BUS_Reactor.h"
#include "SoftwareSerial.h"

#define DEBUG_RX 10
#define DEBUG_TX 11
#define DEVICE_ADDRESS 19


SoftwareSerial myAdemcoSerial(DEBUG_RX,DEBUG_TX);
BUS_Reactor vista20p(&Serial, DEVICE_ADDRESS);

char characterToSend;

void setup() {
  Serial.begin(4800,SERIAL_8E2);
  myAdemcoSerial.begin(4800);
  myAdemcoSerial.println("Setup");
  
  vista20p.attach_display(message_ready);
  vista20p.attach_status(status_upated);
  vista20p.attach_debug(debug_protocol);
  //vista20p.attach_unknown_message(debug_unknown);
}

void loop() {
    vista20p.handleEvents();
        
}

void message_ready(Display_Handler mensaje) {
  myAdemcoSerial.println();
  myAdemcoSerial.print( mensaje.to_string() );     
}

void status_upated(Status_Handler new_status) {
  myAdemcoSerial.println();
  myAdemcoSerial.print( new_status.to_string() );     
}

void debug_protocol(char *mensaje) {
  myAdemcoSerial.println();
  myAdemcoSerial.print( mensaje );     
}

void debug_unknown(char *mensaje) {
  myAdemcoSerial.write( mensaje );     
}

char send_to_keypad() {
    return characterToSend;
}


