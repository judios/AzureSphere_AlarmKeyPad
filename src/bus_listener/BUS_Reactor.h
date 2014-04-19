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
#include "SoftwareSerial2.h"
#include "Event_Handler.h"
#include "Acknowledge_Handler.h"
#include "Display_Handler.h"
#include "Status_Handler.h"
#include "Msg9e_Handler.h"

extern "C" {
    // callback function for Display Messages
    typedef void (*ademcoDisplayCallback)(Display_Handler);
}

extern "C" {
    // callback function for Status updated Messages
    typedef void (*ademcoStatusCallback)(Status_Handler);
}

extern "C" {
    // callback function to allow send to bus
    typedef char (*ademcoClearToSendCallback)();
}

extern "C" {
    // callback function to allow send to bus
    typedef void (*ademcoUnknownCallback)(char *);
}

class BUS_Reactor {
private:
    SoftwareSerial2 *myAdemco;
    HardwareSerial *myAdemcoHardware;

    // Serial Handlers
    Acknowledge_Handler acknowledgeHandler;
    Status_Handler statusHandler;
    Display_Handler displayHandler;
    Msg9e_Handler unkHandler;

    // Notification methods
    void on_acknowledge();
    Stream * getSerialHandler();
    static inline void tunedDelay(uint16_t delay);

    // Callbacks references
    ademcoStatusCallback callbackStatus;
    ademcoDisplayCallback callbackDisplay;
    ademcoClearToSendCallback callbackCTS;
    ademcoDebugProtocolCallback debugCallback;
    ademcoUnknownCallback callbackUnknown;

    // Flag to track send request
    boolean wantToSend;
    boolean softwareSerial;
    
    // Internal Device Address
    int device_address;
    
    int sequence;
    

public:
  // public methods
  BUS_Reactor(SoftwareSerial2 *, int device_address);
  BUS_Reactor(HardwareSerial *,int device_address);
  ~BUS_Reactor();
  void handleEvents();
  void request_to_send();
  void attach_display(ademcoDisplayCallback displayCallback);
  void attach_status(ademcoStatusCallback statusCallback);
  void attach_debug(ademcoDebugProtocolCallback debugCallback);
  void attach_clear_to_send(ademcoClearToSendCallback ctsCallback);
  void attach_unknown_message(ademcoUnknownCallback unknownCallback);
  void deattach_debug();
};

#endif
