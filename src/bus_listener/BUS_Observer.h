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
#ifndef BUS_OBSERVER_H
#define BUS_OBSERVER_H

#include "Arduino.h"
#include "Display_Handler.h"
#include "Status_Handler.h"

extern "C" {
    // callback function for Display Messages
    typedef void (*ademcoSystemArmedCallback)(char *);
}

extern "C" {
    // callback function for Display Messages
    typedef void (*ademcoSystemDisarmedCallback)(char *);
}

extern "C" {
    // callback function for Display Messages
    typedef void (*ademcoAlarmOcurredCallback)(char *);
}

extern "C" {
    // callback function for Display Messages
    typedef void (*ademcoBatteryAlarmOcurredCallback)(char *);
}


class BUS_Observer {
private:
    boolean firstCall;
    boolean armedStay;
    boolean systemReady;
    boolean chime;
    boolean acPower;
    boolean armedAway;
    boolean alarmOcurred; 
    boolean alarmBell;
    
    ademcoSystemArmedCallback callbackAdemcoSystemArmed;
    ademcoSystemDisarmedCallback callbackAdemcoSystemDisarmed;
    ademcoAlarmOcurredCallback callbackAdemcoAlarmOcurred;
    ademcoBatteryAlarmOcurredCallback callbackAdemcoBatteryAlarmOcurred;
public:
  BUS_Observer();
  ~BUS_Observer();
  void update_current_status(Status_Handler currentStatus);
  void update_current_display(Display_Handler currentDisplay);
  void set_system_armed_callback(ademcoSystemArmedCallback);
  void set_system_disarmed_callback(ademcoSystemDisarmedCallback);
  void set_alarm_ocurred_callback(ademcoAlarmOcurredCallback);
  void set_battery_alarm_callback(ademcoBatteryAlarmOcurredCallback);
  
};

#endif
