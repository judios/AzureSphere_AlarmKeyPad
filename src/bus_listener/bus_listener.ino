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
#include <BlynkSimpleEthernet.h>

#define DEVICE_ADDRESS 19
#define LED 9

BUS_Reactor vista20p(&Serial, DEVICE_ADDRESS);

byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0xD7, 0x19 };
char auth[] = "dc97b824634847a5a65a6617b5489361";
WidgetTerminal terminal(V0);
WidgetLCD lcd(V1);
byte ip[] = {10, 1, 1, 190 };

#define BUFFER_LEN 50
char messageBuffer[BUFFER_LEN];

EthernetClient ethClient;
boolean state = false;

BLYNK_WRITE(V0) {
    state = !state;
    digitalWrite(LED, !state ? HIGH : LOW ); // Visualy identify an incomming message
  
    vista20p.request_to_send( param.asStr() );

}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Ethernet.begin(mac, ip);
  Blynk.begin(auth); 
  
  Serial.begin(4800,SERIAL_8N2);
 
  vista20p.attach_display(message_updated);
  vista20p.attach_status(status_upated);
  vista20p.attach_f9(message_f9);
  vista20p.attach_debug(publish_debug_message);
  
  //vista20p.attach_unknown_message(debug_unknown); 
 
}

void loop() {
    
    vista20p.handleEvents();        
    Blynk.run();
    
}

void message_updated(Display_Handler mensaje) {  
    // 32 characters
    mensaje.get_display_message(messageBuffer);

    lcd.clear(); //Use it to clear the LCD Widget

    char subbuff[17];
    strncpy(subbuff, messageBuffer, 16);
    lcd.print(0, 0, subbuff  ); 

    strncpy(subbuff, messageBuffer+16, 16);
    lcd.print(0, 1, subbuff);
  
}


void status_upated(Status_Handler new_status) {
    new_status.to_string(messageBuffer);
    publish_debug_message(messageBuffer);    
}


void message_f9(Msg9e_Handler new_message) {
    new_message.to_string(messageBuffer);
    publish_debug_message(messageBuffer);
}


void publish_debug_message(char *mensaje) {
    terminal.write( mensaje,strlen(mensaje) );
    terminal.println();
    terminal.flush();
}

void debug_unknown(char *mensaje) {
  
}


