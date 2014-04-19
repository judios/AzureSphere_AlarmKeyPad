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
#include "BUS_Reactor.h"

/* static */ 
inline void BUS_Reactor::tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+w" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

/*
 * calls the callback method callback clear to send
 * so the client can write to the BUS
 */
void BUS_Reactor::on_acknowledge() {
  
  if ( acknowledgeHandler.get_ack_address() == device_address ) {
        // IF there is no callback exit without processing
        if ( callbackCTS == NULL ) {
            return;
        }
        
        char request = (*callbackCTS)();
        
        if ( request == NULL ) {
            return;
        }
        
        char buffer[16];
        memset(buffer, 0x00, sizeof(buffer));
        
        char header;
        int bufferIndex = 0;
        
        // 2 MSB represent the Seq
        // 6 LSB represent address
        // 1100 0000 0xc0
        // 0011 1111 0x3f
        header = (((sequence++<<6) & 0xc0) ^ 0xc0) | (device_address & 0x3f);

        buffer[bufferIndex++] = header; // first byte
        buffer[bufferIndex++] = 2; // message length
        
        char chksum = 0x100;
        chksum = chksum - header - 2;

        buffer[bufferIndex++] = request; // kestroke
            
        chksum = chksum - request;

            
        buffer[bufferIndex++] = chksum & 0xff; // checksum
        /*
        acknowledgeHandler.on_debug( buffer, bufferIndex );
        
        for ( int i = 0; i < (bufferIndex-1); i++ ) {
          myAdemco->writeParity( buffer[i] );
        }
        */
        
  }

}

/********************************************
 *  PUBLIC Methods
 *
 ********************************************/

/*
 * Constructor
 * Initializes Software Serial Port at 4800 Bds 8E2
 *
 */
BUS_Reactor::BUS_Reactor(SoftwareSerial2 *myAdemcoSerial, int device_address_parm) { 
    myAdemco = myAdemcoSerial;
    wantToSend = false;
    softwareSerial = true;
    sequence = 1;
    device_address = device_address_parm;
    callbackDisplay = NULL;
    callbackStatus = NULL;  
    callbackCTS = NULL;

    acknowledgeHandler.set_serial_handler(myAdemco);
    statusHandler.set_serial_handler(myAdemco);
    displayHandler.set_serial_handler(myAdemco);
    unkHandler.set_serial_handler(myAdemco);
}

/*
 * Constructor
 * Initializes Hardware Serial Port at 4800 Bds 8E2
 *
 */
BUS_Reactor::BUS_Reactor(HardwareSerial *myAdemcoSerial, int device_address_parm) { 
    myAdemcoHardware = myAdemcoSerial;
    wantToSend = false;
    softwareSerial = false;
    sequence = 1;
    device_address = device_address_parm;
    callbackDisplay = NULL;
    callbackStatus = NULL;  
    callbackCTS = NULL;

    acknowledgeHandler.set_serial_handler(myAdemcoHardware);
    statusHandler.set_serial_handler(myAdemcoHardware);
    displayHandler.set_serial_handler(myAdemcoHardware);
    unkHandler.set_serial_handler(myAdemcoHardware);
}

/*
 * Destructor
 */
BUS_Reactor::~BUS_Reactor() {
    callbackDisplay = NULL;
    callbackStatus = NULL;
    callbackCTS = NULL;
}

Stream * BUS_Reactor::getSerialHandler() {
  if ( softwareSerial == true )  {
    return myAdemco;
  } else {
    return myAdemcoHardware;
  }
  
}
    
    
/*
 * handle Serial events must be called inside the loop method
 */
void BUS_Reactor::handleEvents() {
    // handle Reception
    if ( getSerialHandler()->available() > 0 ) {
      char cr = getSerialHandler()->read();
      
      if ( cr == F7_DISPLAY_EVENT ) {

        if ( displayHandler.handle_event( F7_DISPLAY_EVENT ) > 0 ) {
            // IF there is no callback exit without processing
            if ( callbackDisplay == NULL ) {
                return;
            }
            (*callbackDisplay)(displayHandler);
        }
        
      } else if ( cr == F6_ACK_EVENT ) {

        if ( acknowledgeHandler.handle_event( F6_ACK_EVENT ) > 0 ) {
            on_acknowledge();
        }
        
      } else if ( cr == F2_STATUS_EVENT ) {

        if ( statusHandler.handle_event( F2_STATUS_EVENT ) > 0 ) {
            // IF there is no callback exit without processing
            if ( callbackStatus == NULL ) {
                return;
            }
            (*callbackStatus)(statusHandler);
        }
        
      } else if ( cr == F9E_UNK_EVENT ) {
        
        if ( unkHandler.handle_event( F9E_UNK_EVENT ) > 0 ) {
            
        }
        
      }  else {
        // Unknown messages or cero
        if ( callbackUnknown == NULL ) {
          return;
        }
        
        char buffer[12];
        memset(buffer, 0x00, sizeof(buffer));
        
        sprintf( buffer, "[%02x]", cr );
               
        (*callbackUnknown)(buffer);

      }

    // No incomming data ? check if transmition is needed
    } else {
      /*
        if ( wantToSend == true ) {
            unsigned long duration;
            
            uint8_t oldSREG = SREG;
            cli();  // turn off interrupts for a clean txmit

            //duration = pulseIn( 7, HIGH ); 
                        
            // Waits on pin1 for a HIGH value is at least 12 millisec            
            //if ( duration >= 12 * 1000 ) {                  
                myAdemco->write( 0xff );
                delay(100);
              //  pulseIn( 7, HIGH );
                myAdemco->write( 0xff );
                delay(100);
              //  pulseIn( 7, HIGH );
                myAdemco->write( 0xfe );

              //  wantToSend = false;               
            //}
            delay(500);
            
            
            SREG = oldSREG; // turn interrupts back on
        }
      */
    }
}


void BUS_Reactor::request_to_send() {
    wantToSend = true;
}

void BUS_Reactor::attach_display(ademcoDisplayCallback displayCallback) {
    callbackDisplay = displayCallback;
}

void BUS_Reactor::attach_status(ademcoStatusCallback statusCallback) {
    callbackStatus = statusCallback;
}

void BUS_Reactor::attach_debug(ademcoDebugProtocolCallback debugCallbackParam) {
    debugCallback = debugCallbackParam;
    
    if ( debugCallback != NULL ) {
        acknowledgeHandler.enable_debug( debugCallback );
        statusHandler.enable_debug( debugCallback );
        displayHandler.enable_debug( debugCallback );
        unkHandler.enable_debug( debugCallback );
    }
}

void BUS_Reactor::attach_clear_to_send(ademcoClearToSendCallback ctsCallback) {
    callbackCTS = ctsCallback;
}

void BUS_Reactor::attach_unknown_message(ademcoUnknownCallback unknownCallback) {
    callbackUnknown = unknownCallback;
}

void BUS_Reactor::deattach_debug() {
    acknowledgeHandler.disable_debug();
    statusHandler.disable_debug();
    displayHandler.disable_debug();
    unkHandler.disable_debug();
}
