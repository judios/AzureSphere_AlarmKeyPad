/*
    Copyright 2013 Jose Castellanos Molina
    
	Modified in 2019 for use with AlarmKeyPad project by Julian Diaz
    
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
#pragma once

#include "..\AlarmKeyPad.h"
#include "Event_Handler.h"
#include "Acknowledge_Handler.h"
#include "Display_Handler.h"
#include "Status_Handler.h"
#include "Msg9e_Handler.h"

#define KEY_MESSAGE_LEN  20

// callback function for Display Messages
typedef void (*panelDisplayCallback)();

// callback function for Status updated Messages
typedef void (*panelStatusCallback)();

// callback function for Status updated Messages
typedef void (*panelF9Callback)();

// callback function to allow send to bus
typedef void (*panelDebugProtocolCallback)(char *);



    // Notification methods
void busReactor_on_acknowledge();
void busReactor_acknowledgeAddress();      
    
void busReactor_Init(int device_address);

  void busReactor_handleEvents();
  void busReactor_request_to_send(const char *);
  void busReactor_attach_display(panelDisplayCallback displayCallback);
  void busReactor_attach_status(panelStatusCallback statusCallback);
  void busReactor_attach_f9(panelF9Callback callbackF9);
  void busReactor_attach_debug(panelDebugProtocolCallback debugCallback);
  void busReactor_deattach_debug();

