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
#ifndef BUS_REACTOR_h
#define BUS_REACTOR_h

#include "Arduino.h"
#include "Event_Handler.h"
#include "Acknowledge_Handler.h"
#include "Display_Handler.h"
#include "Status_Handler.h"
#include "Msg9e_Handler.h"

int const KEY_MESSAGE_LEN = 20;

extern "C" {
    // callback function for Display Messages
    typedef void (*panelDisplayCallback)(Display_Handler);
}

extern "C" {
    // callback function for Status updated Messages
    typedef void (*panelStatusCallback)(Status_Handler);
}

extern "C" {
    // callback function for Status updated Messages
    typedef void (*panelF9Callback)(Msg9e_Handler);
}

extern "C" {
    // callback function to allow send to bus
    typedef void (*panelDebugProtocolCallback)(char *);
}

class BUS_Reactor {
private:
    HardwareSerial *myAdemcoHardware;

    // Serial Handlers
    Acknowledge_Handler acknowledgeHandler;
    Status_Handler statusHandler;
    Display_Handler displayHandler;
    Msg9e_Handler unkHandler;

    // Notification methods
    void on_acknowledge();
    HardwareSerial * getSerialHandler();
    unsigned long pulseInBUS(uint8_t pin, uint8_t state);
    

    // Callbacks references
    panelStatusCallback callbackStatus;
    panelDisplayCallback callbackDisplay;
    panelDebugProtocolCallback debugCallback;
    panelF9Callback callbackF9;

    // Flag to track send request
    boolean wantToSend;
    
    // Internal Device Address
    int device_address;
    
    int sequence;
    
    char keys_to_send[KEY_MESSAGE_LEN];        
    
public:
  // public methods
  BUS_Reactor(HardwareSerial *,int device_address);
  ~BUS_Reactor();
  void handleEvents();
  void request_to_send(char *);
  void attach_display(panelDisplayCallback displayCallback);
  void attach_status(panelStatusCallback statusCallback);
  void attach_f9(panelF9Callback callbackF9);
  void attach_debug(panelDebugProtocolCallback debugCallback);
  void deattach_debug();
};

#endif
