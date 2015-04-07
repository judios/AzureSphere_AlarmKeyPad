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
#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Arduino.h"

int const F2_STATUS_EVENT = 0xFFF2;
int const F6_ACK_EVENT = 0xFFF6;
int const F7_DISPLAY_EVENT = 0xFFF7;
int const F9E_UNK_EVENT = 0xFFF9;

extern "C" {
    // callback function for Status updated Messages
    typedef void (*panelDebugProtocolCallback)(char *);
}

class Event_Handler {
private:
  Stream *myAdemco;
  boolean debugProtocol;
  panelDebugProtocolCallback callbackDebugProtocol;
protected:
  void read_chars(int ct, char *bufferHex );
  int read_chars_dyn(char *bufferHex);
public:
  Event_Handler();
  virtual int handle_event (char et) = 0;
  virtual void to_string(char *) = 0;
  void on_debug(char *, int );
  void enable_debug(panelDebugProtocolCallback callbackDebugProtocol);
  void disable_debug();
  void set_serial_handler(Stream *myAdemco);
};

#endif
