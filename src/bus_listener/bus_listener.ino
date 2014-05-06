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
#define       NO_PORTB_PINCHANGES
#define       NO_PORTC_PINCHANGES
//#define       NO_PORTD_PINCHANGES
#include <PinChangeInt.h>

#include "BUS_Reactor.h"
#include "SoftwareSerial.h"

#define DEBUG_RX 10
#define DEBUG_TX 11
#define DEVICE_ADDRESS 16

SoftwareSerial myAdemcoSerial(DEBUG_RX,DEBUG_TX);
BUS_Reactor vista20p(&Serial, DEVICE_ADDRESS);

char characterToSend;
int addressCharacters[] = {0xff, 0xff, 0xfe };
int addressCount = 0;
boolean pulseInDetected;
unsigned long pulseStart = 0;
unsigned long pulseTime = 0;

void risingEdgeCall() {
    pulseInDetected = false;
    pulseStart = micros();
    
    PCintPort::detachInterrupt(0);
    PCintPort::attachInterrupt(0, &fallingEdgeCall, FALLING); //RISING,FALLING, CHANGE
}

void fallingEdgeCall() {  
    pulseTime = micros() - pulseStart;
    pulseInDetected = true;
    
    PCintPort::detachInterrupt(0);
}

void detectPulse() {
      pulseStart = 0;
      pulseTime = 0;
      pulseInDetected = false;
      PCintPort::detachInterrupt(0);
      PCintPort::attachInterrupt(0, &risingEdgeCall, RISING); //RISING,FALLING, CHANGE
}



void writeAdrress() {
      micros();
      PCintPort::detachInterrupt(0);
      PCintPort::attachInterrupt(0, &writeOnRisingEdge, RISING); //RISING,FALLING, CHANGE
}

void writeOnRisingEdge() {
      micros();
      PCintPort::detachInterrupt(0);
      PCintPort::attachInterrupt(0, &writeOnFalingEdge, FALLING); //RISING,FALLING, CHANGE
}

void writeOnFalingEdge() {
      Serial.write( addressCharacters[ addressCount ] );
      addressCount++;
      
      if ( addressCount != 3 ) {
        writeAdrress();
        //PCintPort::detachInterrupt(0);
        //PCintPort::attachInterrupt(0, &writeOnFalingEdge, FALLING); //RISING,FALLING, CHANGE
      } else {
        PCintPort::detachInterrupt(0);
        Serial.begin(4800,SERIAL_8E2);
        addressCount = 0;
      }
}

void setup() {
  Serial.begin(4800,SERIAL_8E2);
  myAdemcoSerial.begin(4800);
  myAdemcoSerial.println("Setup");
  
  //vista20p.attach_display(message_ready);
  //vista20p.attach_status(status_upated);
  vista20p.attach_debug(debug_protocol);
  //vista20p.attach_unknown_message(debug_unknown); 
  //vista20p.attach_clear_to_send(send_to_keypad);
  
}

void loop() {
    vista20p.handleEvents();        
    
    
    if ( myAdemcoSerial.available() ) {
        characterToSend = myAdemcoSerial.read();
        
        if ( characterToSend == '1' ) {
          characterToSend = 0x01;
        } else if ( characterToSend == '2' ) {
          characterToSend = 0x02;
        } else if ( characterToSend == '3' ) {
          characterToSend = 0x03;
        } else if ( characterToSend == '4' ) {
          characterToSend = 0x04;
        } else if ( characterToSend == '5' ) {
          characterToSend = 0x05;
        } else if ( characterToSend == '6' ) {
          characterToSend = 0x06;
        } else if ( characterToSend == '7' ) {
          characterToSend = 0x07;
        } else if ( characterToSend == '8' ) {
          characterToSend = 0x08;
        } else if ( characterToSend == '9' ) {
          characterToSend = 0x09;
        } else if ( characterToSend == '0' ) {
          characterToSend = 0x00;
        }
      
      //detectPulse(); 
      vista20p.request_to_send();
    }
    
    
    if ( pulseInDetected == true ) {
      pulseInDetected = false;    
    
      if ( pulseTime > 12 * 1000 ) {   
        addressCount = 0;
        
        Serial.begin(4800,SERIAL_8N2);     
        Serial.write( addressCharacters[addressCount] );
              
        addressCount = 1;
        
        writeAdrress();
        //PCintPort::detachInterrupt(0);
        //PCintPort::attachInterrupt(0, &writeOnFalingEdge, FALLING); //RISING,FALLING, CHANGE

      } else {
        detectPulse();  
      }    
    
    }
    
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


