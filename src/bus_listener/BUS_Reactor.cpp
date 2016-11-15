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


/*
 * calls the callback method callback clear to send
 * so the client can write to the BUS
 */
void BUS_Reactor::on_acknowledge() {
  
  if ( debugCallback != NULL ) {
      char auxBuffer[10];
      acknowledgeHandler.to_string( auxBuffer );
      (*debugCallback)( auxBuffer );
  }
  
  if ( acknowledgeHandler.get_ack_address() == device_address ) {

        int charsSend = strlen( keys_to_send );
        
        if ( charsSend > 0 && charsSend < KEY_MESSAGE_LEN ) {
                    
          getSerialHandler()->begin(4800,SERIAL_8E1);
          
          int header;
          int bufferIndex = 0;
          
          // 2 MSB represent the Seq
          // 6 LSB represent address
          // 1100 0000 0xc0
          // 0011 1111 0x3f
          header = (((++sequence<<6) & 0xc0) ^ 0xc0) | (device_address & 0x3f);
          getSerialHandler()->write( (int)header );
          getSerialHandler()->write( (int)charsSend+1 ); // message length
          
          int chksum = 0x00;
          for (int i = 0; i < charsSend ; i++ ) {
            // [0-9]
            if(keys_to_send[i] >= 0x30 && keys_to_send[i] <= 0x39 ) {
              getSerialHandler()->write( (int)(keys_to_send[i] - 0x30) ) ; 
              chksum += ( keys_to_send[i] - 0x30 );
            // [*]
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
                        
          }
              
          getSerialHandler()->write( (int)((0x100 - header - (charsSend + 1) - chksum ) & 0xff) ); // checksum
          getSerialHandler()->flush();

          // SERIAL_8E1 Data should be written 8 Data 1 Parity(Even) 1 Stop Bits
          getSerialHandler()->begin(4800,SERIAL_8N2);

          char messageBuffer[120];
          snprintf( messageBuffer, 120, "!ACK:[%x][%x][%s][%x]", header, (charsSend+1), keys_to_send, ((0x100 - header - (charsSend + 1) - chksum ) & 0xff) );  
          (*debugCallback)( messageBuffer );
                    
          memset(keys_to_send, 0x00, sizeof(keys_to_send));
    }
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
  
unsigned long BUS_Reactor::pulseInBUS(uint8_t pin, uint8_t state)  {
    // cache the port and bit of the pin in order to speed up the
    // pulse width measuring loop and achieve finer resolution.  calling
    // digitalRead() instead yields much coarser resolution.
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    uint8_t stateMask = (state ? bit : 0);
    unsigned long width = 0; // keep initialization out of time critical area
	
    // convert the timeout from microseconds to a number of times through
    // the initial loop; it takes 16 clock cycles per iteration.
    unsigned long numloops = 0;
    unsigned long maxloops = 5000000;
	
    // wait for any previous pulse to end
    while ((*portInputRegister(port) & bit) == stateMask)
  	if (numloops++ == maxloops)
        	return 0;
	
    // wait for the pulse to start
    while ((*portInputRegister(port) & bit) != stateMask)
	if (numloops++ == maxloops)
		return 0;
	
    // wait for the pulse to stop
    while ((*portInputRegister(port) & bit) == stateMask) {
	width++;
    }

    return width;
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
              
            }  else {
              // Unknown messages or cero
              if ( debugCallback == NULL ) {
                return;
              }
              
              //char buffer[12];
              //memset(buffer, 0x00, sizeof(buffer));
              
              //sprintf( buffer, "[%02x]", cr );
                     
              //(*debugCallback)(buffer);
      
            }
      
          // No incomming data ? check if transmition is needed
          } 
     } else if ( wantToSend == true ) {   
          // SERIAL_8N2 Adress Data should be written 8 Data 2 Stop Bit No Parity      
                  
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
              */
            
              getSerialHandler()->write( 0xff ); 
              pulseIn( 0, HIGH ); 

              getSerialHandler()->write( 0xff );
              pulseIn( 0, HIGH );
                
              getSerialHandler()->write( 0xf7 ); 
       
              wantToSend = false;                     
          }
                        
      }
          
}

void BUS_Reactor::request_to_send(char *messageToKeyPad) {
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
