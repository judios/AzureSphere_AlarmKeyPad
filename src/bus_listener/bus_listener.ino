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
#include "SoftwareSerial2.h"

#define ADEMCO_RX 7
#define ADEMCO_TX 6
#define DEVICE_ADDRESS 19

SoftwareSerial myAdemcoSerial(ADEMCO_RX,ADEMCO_TX);
BUS_Reactor vista20p(&myAdemcoSerial, DEVICE_ADDRESS);
char characterToSend;

void setup() {
  myAdemcoSerial.begin(4800);
  
  Serial.begin(4800);
  Serial.println("Setup");
  
  vista20p.attach_display(message_ready);
  vista20p.attach_status(status_upated);
  vista20p.attach_debug(debug_protocol);
  vista20p.attach_clear_to_send(send_to_keypad);

}

void loop() {
    vista20p.handleEvents();
    
    if ( Serial.available() > 0 ) {
        characterToSend = Serial.read();
        Serial.println( characterToSend );
        vista20p.request_to_send();
    }
}

void message_ready(Display_Handler mensaje) {
  Serial.println( mensaje.to_string() );     
}

void status_upated(Status_Handler new_status) {
  Serial.println( new_status.to_string() );     
}

void debug_protocol(char *mensaje) {
  Serial.println( mensaje );     
}

char send_to_keypad() {
    return characterToSend;
}

