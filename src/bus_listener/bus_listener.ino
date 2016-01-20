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
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define DEVICE_ADDRESS 19
#define LED 9

BUS_Reactor vista20p(&Serial, DEVICE_ADDRESS);

char characterToSend;

byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0xD7, 0x19 };
char clientName[] = "arduino:mty:90a2da0dd719";
byte ip[] = {10, 1, 1, 190 };

char servername[] ="test.mosquitto.org";
char topicNamePublish[] = "alarmpaneltopic/1";
char topicNameReceive[] = "alarmpaneltopic/2";
#define BUFFER_LEN 300
char messageBuffer[BUFFER_LEN];

EthernetClient ethClient;
boolean state = false;
boolean debug = false;

void callback(char* topic, byte* payload, unsigned int length) {
    state = !state;
    digitalWrite(LED, !state ? HIGH : LOW ); // Visualy identify an incomming message
    char auxBuffer[20];

    if ( length <= sizeof(auxBuffer) ) {
        
      memset(auxBuffer, 0x00, sizeof(auxBuffer));
      memcpy(auxBuffer, payload, length);
         
      memset(messageBuffer, 0x00, sizeof(messageBuffer));
      snprintf( messageBuffer,BUFFER_LEN, "!CBK:[%s][%s]", topic, auxBuffer );  
      
      //publish_debug_message(messageBuffer);
   
      if ( strcmp("status_on",auxBuffer ) == 0 ) {
         vista20p.attach_status(status_upated);
      // Disable Status
      } else if ( strcmp("status_off",auxBuffer ) == 0 ) {
         vista20p.attach_status( NULL );
      // Enable Display
      } else if ( strcmp("display_on",auxBuffer ) == 0 ) {
         vista20p.attach_display(message_ready);
      // Disable Display
      } else if ( strcmp("display_off",auxBuffer ) == 0 ) {
         vista20p.attach_display( NULL );
      // Enable RFX Messages
      } else if ( strcmp("rfx_on",auxBuffer ) == 0 ) {
         vista20p.attach_f9(message_f9);
      // Disable RFX Messages
      } else if ( strcmp("rfx_off",auxBuffer ) == 0 ) {
         vista20p.attach_f9( NULL );
      } else if ( strcmp("debug_on",auxBuffer ) == 0 ) {
         debug = true;
      } else if ( strcmp("debug_off",auxBuffer ) == 0 )  {
         debug = false;
      }
      
      characterToSend = '*';
      vista20p.request_to_send();
      
    }
}

PubSubClient client(servername, 1883, callback, ethClient);

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Ethernet.begin(mac, ip);
  
  Serial.begin(4800,SERIAL_8N2);
 
  vista20p.attach_display(message_ready);
  //vista20p.attach_status(status_upated);
  //vista20p.attach_f9(message_f9);
  vista20p.attach_debug(publish_debug_message);
  
  //vista20p.attach_unknown_message(debug_unknown); 
  vista20p.attach_clear_to_send(send_to_keypad);  
  
}

void loop() {
    
    vista20p.handleEvents();        
    
    if (!client.connected()) {    
        client.connect(clientName);
        client.publish(topicNamePublish,"Connected");
        client.subscribe(topicNameReceive);
    }
    
    client.loop();
}

// 74 Characters
void message_ready(Display_Handler mensaje) {
  if (client.connected() ) {      
      mensaje.to_string(messageBuffer);
      client.publish(topicNamePublish,messageBuffer);

      if ( debug == true ) {
         mensaje.debug_to_string(messageBuffer);
         client.publish(topicNamePublish,messageBuffer);
      }
  }
}

//287 Characters
void status_upated(Status_Handler new_status) {
  if (client.connected() ) {
      new_status.to_string(messageBuffer);
      client.publish(topicNamePublish,messageBuffer);

      if ( debug == true ) {
         new_status.debug_to_string(messageBuffer);
         client.publish(topicNamePublish,messageBuffer);
      }
  }
}

//37 Characters
void message_f9(Msg9e_Handler new_message) {
  if (client.connected() ) {
      new_message.to_string(messageBuffer);
      client.publish(topicNamePublish,messageBuffer);

      if ( debug == true ) {
         new_message.debug_to_string(messageBuffer);
         client.publish(topicNamePublish,messageBuffer);
      }
  }
}

// 517 Characters
void publish_debug_message(char *mensaje) {
  if (client.connected() ) {      
      client.publish(topicNamePublish,mensaje);
  }    
}

void debug_unknown(char *mensaje) {
  if (client.connected() ) {      
      client.publish(topicNamePublish,mensaje);
  }  
}

char send_to_keypad() {
    return characterToSend;
}



