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

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif  

/*
 * Called after and F6 even to gice us the chance to
 * write to the BUS, if it is our address
 */
void BUS_Reactor::on_acknowledge() {
  
  if ( acknowledgeHandler.get_ack_address() == device_address ) {
        sbi(UCSR0C, UPM01 ); // Enable Parity
        cbi(UCSR0C, UPM00 ); // Even Parity
        
        int charsSend = strlen( keys_to_send );
        
        if ( charsSend > 0 && charsSend < KEY_MESSAGE_LEN ) {          
          int bufferIndex = 0;
          
          // 2 MSB represent the Seq
          // 6 LSB represent address
          // 1100 0000 0xc0
          // 0011 1111 0x3f
          header = (((++sequence<<6) & 0xc0) ^ 0xc0) | (device_address & 0x3f);
          getSerialHandler()->write( (int)header );
          getSerialHandler()->flush();
          getSerialHandler()->write( (int)charsSend+1 ); // message length
          getSerialHandler()->flush();
          
          int chksum = 0x00;
          for (int i = 0; i < charsSend ; i++ ) {
            // [0-9]
            if(keys_to_send[i] >= 0x30 && keys_to_send[i] <= 0x39 ) {
              getSerialHandler()->write( (int)(keys_to_send[i] - 0x30) ) ; 
              chksum += ( keys_to_send[i] - 0x30 );
            // [#]
            } else if ( keys_to_send[i] == 0x23 ) {
              getSerialHandler()->write( (int)0x0B ); 
              chksum += 0x0B;
            // [*]
            } else if ( keys_to_send[i] == 0x2A ) {
              getSerialHandler()->write( (int)0x0A ); 
              chksum += 0x0A;
            // [A B C D]
            } else if(keys_to_send[i] >= 0x41 && keys_to_send[i] <= 0x44 ) {
              getSerialHandler()->write( (int)(keys_to_send[i] - 0x25) ) ; 
              chksum += ( keys_to_send[i] - 0x25 );
            }
            getSerialHandler()->flush();           
          }
              
          getSerialHandler()->write( (int)((0x100 - header - (charsSend + 1) - chksum ) & 0xff) ); // checksum
          getSerialHandler()->flush();

        }

        cbi(UCSR0C, UPM01 ); // Disable Parity
        cbi(UCSR0C, UPM00 ); // Even Parity
        memset(keys_to_send, 0x00, sizeof(keys_to_send));
    
        // Used to identify Acknowledge 
        if ( debugCallback != NULL ) {
            char auxBuffer[16];
            sprintf(auxBuffer,"!SND:[%02x]", header );
            (*debugCallback)( auxBuffer );
        }
  }

  if ( debugCallback != NULL ) {
      char auxBuffer[16];
      acknowledgeHandler.to_string( auxBuffer );
      (*debugCallback)( auxBuffer );
  }

}

/********************************************
 *  PUBLIC Methods
 *
 ********************************************/

/*
 * Constructor
 * Initializes Hardware Serial Port at 4800 Bds 8N2
 *
 */
BUS_Reactor::BUS_Reactor(HardwareSerial *myAdemcoSerial, int device_address_parm) { 
    myAdemcoHardware = myAdemcoSerial;
    wantToSend = false;
    sequence = 1;
    header = 0x00;
    device_address = device_address_parm;
    callbackDisplay = NULL;
    callbackStatus = NULL;  

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
}

HardwareSerial * BUS_Reactor::getSerialHandler() {
    return myAdemcoHardware;
  
}
  
/*
 * handle Serial events must be called inside the loop method
 */
void BUS_Reactor::handleEvents() {
    
    if ( wantToSend == false ) {
          // handle Reception
          if ( getSerialHandler()->available() > 0 ) {
            char cr = getSerialHandler()->read();
            
            if ( cr == F7_DISPLAY_EVENT ) {
      
              if ( displayHandler.handle_event( F7_DISPLAY_EVENT ) > 0 ) {
                  acknowledgeAddress();
                  
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
                           
                    if ( callbackF9 == NULL ) {
                      return;
                    }
                    (*callbackF9)(unkHandler);
              }
              
            } else if ( header != 0x00 && cr == header ) {
                header = 0x00;
                if ( debugCallback == NULL ) {
                      return;
                }

                char auxBuffer[12];
                sprintf(auxBuffer,"!MACK:[%02x]", cr );
                (*debugCallback)( auxBuffer );

            } else {
              // Unknown messages or cero
              if ( debugCallback == NULL && cr != 0x00 ) {
                return;
              }
              
              //char auxBuffer[4];
              //sprintf("%02x,", cr );
              //(*debugCallback)( auxBuffer );              
      
            }
      
          // No incomming data ? check if transmition is needed
          } 
     } else if ( wantToSend == true ) {   
          
          acknowledgeAddress();
                             
     }
          
}

void BUS_Reactor::acknowledgeAddress() {
  // Waits on pin1 for a HIGH value is at least 12 millisec            
  if ( pulseIn( 0, HIGH ) >= 8000 ) {    
              /*        LSB                       MSB
                        1248 1248 1248 1248 1248 1248
          16 FF,FF,FE   1111 1111 1111 1111 0111 1111
          17 FF,FF,FD   1111 1111 1111 1111 1011 1111
          18 FF,FF,FB   1111 1111 1111 1111 1101 1111
          19 FF,FF,F7   1111 1111 1111 1111 1110 1111 
          20 FF,FF,EF   1111 1111 1111 1111 1111 0111
          21 FF,FF,DF   1111 1111 1111 1111 1111 1011
          22 FF,FF,BF   1111 1111 1111 1111 1111 1101 
          23 FF,FF,FE   1111 1111 1111 1111 1111 1110 
          24 FF,FF,FF   1111 1111 1111 1111 1111 1111
          */
            
              getSerialHandler()->write( 0xff ); 
              pulseIn( 0, HIGH ); 

              getSerialHandler()->write( 0xff );
              pulseIn( 0, HIGH );
                
              getSerialHandler()->write( ~(0x01 << (device_address-16) ) ); 
       
              wantToSend = false;   
  }
}

void BUS_Reactor::request_to_send(const char *messageToKeyPad) {
    memset(keys_to_send, 0x00, sizeof(keys_to_send));
    strncpy(keys_to_send, messageToKeyPad, KEY_MESSAGE_LEN);
    wantToSend = true;
}

void BUS_Reactor::attach_display(panelDisplayCallback displayCallback) {
    callbackDisplay = displayCallback;
}

void BUS_Reactor::attach_status(panelStatusCallback statusCallback) {
    callbackStatus = statusCallback;
}

void BUS_Reactor::attach_f9(panelF9Callback f9Callback ) {
    callbackF9 = f9Callback;
}

void BUS_Reactor::attach_debug(panelDebugProtocolCallback debugCallbackParam) {
    debugCallback = debugCallbackParam;
    
    if ( debugCallback != NULL ) {
        //acknowledgeHandler.enable_debug( debugCallback );
        //statusHandler.enable_debug( debugCallback );
        //displayHandler.enable_debug( debugCallback );
        //unkHandler.enable_debug( debugCallback );
    }
}

void BUS_Reactor::deattach_debug() {
    // debugCallback = NULL;
}
