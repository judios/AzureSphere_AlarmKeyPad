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
#include "SoftwareSerial2.h"

int const F2_STATUS_EVENT = 0xFFF2;
int const F6_ACK_EVENT = 0xFFF6;
int const F7_DISPLAY_EVENT = 0xFFF7;

extern "C" {
    // callback function for Status updated Messages
    typedef void (*ademcoDebugProtocolCallback)(char *);
}

class Event_Handler {
private:
  Stream *myAdemco;
  boolean debugProtocol;
  ademcoDebugProtocolCallback callbackDebugProtocol;
protected:
  void read_chars(int ct, char *bufferHex );
  int read_chars_dyn(char *bufferHex);
public:
  Event_Handler();
  virtual int handle_event (char et) = 0;
  virtual char * to_string() = 0;
  void on_debug(char *, int );
  void enable_debug(ademcoDebugProtocolCallback callbackDebugProtocol);
  void disable_debug();
  void set_serial_handler(Stream *myAdemco);
};

#endif
