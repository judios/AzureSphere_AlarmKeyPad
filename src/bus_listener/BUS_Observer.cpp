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
#include "BUS_Observer.h"

/*
 * Constructor with the reference to the input handler
 */
BUS_Observer::BUS_Observer() {
    firstCall = true;
    armedStay = false;
    systemReady = false;
    chime = false;
    acPower = false;
    armedAway = false;
    alarmOcurred = false; 
    alarmBell = false;
}

BUS_Observer::~BUS_Observer() {

}

void BUS_Observer::update_current_status(Status_Handler currentStatus) {        
    
}

void BUS_Observer::update_current_display(Display_Handler currentDisplay) {
  // Compare with previous values
    if ( firstCall == false && armedStay != currentDisplay.get_armed_stay() ) {
        (*callbackAdemcoSystemArmed)(currentDisplay.to_string());
    }
    
    if ( firstCall == false && systemReady != currentDisplay.get_ready() ) {
        (*callbackAdemcoSystemDisarmed)(currentDisplay.to_string());        
    }
    
    if ( firstCall == false && chime != currentDisplay.get_chime_mode() ) {
        (*callbackAdemcoSystemDisarmed)(currentDisplay.to_string());
    }
    
    if ( firstCall == false && acPower != currentDisplay.get_ac_power() ) {
        (*callbackAdemcoBatteryAlarmOcurred)(currentDisplay.to_string());
    }
    
    if ( firstCall == false && armedAway != currentDisplay.get_armed_away() ) {
        (*callbackAdemcoSystemArmed)(currentDisplay.to_string());
    }
    
    if ( firstCall == false && alarmOcurred != currentDisplay.get_alarm_ocurred() ) {
        
    }
    
    if ( firstCall == false && alarmBell != currentDisplay.get_alarm_bell() ) {
        (*callbackAdemcoAlarmOcurred)(currentDisplay.to_string());
    }
    
    
    // Store new values as previous values
    armedStay = currentDisplay.get_armed_stay();
    systemReady = currentDisplay.get_ready();
    chime = currentDisplay.get_chime_mode();
    acPower = currentDisplay.get_ac_power();
    armedAway = currentDisplay.get_armed_away();
    alarmOcurred = currentDisplay.get_alarm_ocurred(); // 
    alarmBell = currentDisplay.get_alarm_bell();
  
    //currentStatus.get_display_message();
  

    firstCall = false;
  
}

void BUS_Observer::set_system_armed_callback(ademcoSystemArmedCallback callback) {
    callbackAdemcoSystemArmed = callback;
    
}

void BUS_Observer::set_system_disarmed_callback(ademcoSystemDisarmedCallback callback) {
    callbackAdemcoSystemDisarmed = callback;
    
}

void BUS_Observer::set_alarm_ocurred_callback(ademcoAlarmOcurredCallback callback) {
    callbackAdemcoAlarmOcurred = callback;
   
}

void BUS_Observer::set_battery_alarm_callback(ademcoBatteryAlarmOcurredCallback callback){
    callbackAdemcoBatteryAlarmOcurred = callback;
}

