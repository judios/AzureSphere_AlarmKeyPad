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

#include "Display_Handler.h"

/*********************************************
 *       STRUCT TO HANDLE F7 MESSAGES
 *********************************************/
 // F7 Message
#define F7_MESSAGE_LEN 45

int const F7_BM_BYTE6_BEEP = 0x07;
int const F7_BM_BYTE6_NIGHT_MODE = 0x10;

int const F7_BM_BYTE7_MESSAGE = 0x00;
int const F7_BM_BYTE7_READY = 0x10;
int const F7_BM_BYTE7_ARMED_STAY = 0x80;

int const F7_BM_BYTE8_CHIME_MODE = 0x20;
int const F7_BM_BYTE8_PROGRAM = 0x40;

int const F7_BM_BYTE8_ALARM = 0x01;
int const F7_BM_BYTE8_FAULT = 0x02;
int const F7_BM_BYTE8_ARMED_AWAY = 0x04;
int const F7_BM_BYTE8_AC_POWER = 0x08;
int const F7_BM_BYTE8_ENTRY_DELAY = 0x80;

char buffer[F7_MESSAGE_LEN + 1];

void displayHandler_Init(){
    
}

int displayHandler_handle_event(char et){
	displayHandler_reset();
    buffer[0] = et;
    read_chars( F7_MESSAGE_LEN, buffer );
    return 1;
}

void displayHandler_reset(){
    memset(buffer, 0x00, sizeof(buffer));
}

int displayHandler_get_addr1() {
    return buffer[1];
}

int displayHandler_get_addr2() {
    return buffer[2];
}

int displayHandler_get_addr3() {
    return buffer[3];
}

int displayHandler_get_addr4() {
    return buffer[4];
}

/* Represents when a zone is faulted the zone number
 * or when in programming the current field
 */
int displayHandler_get_zone_field() {
    return buffer[5];
}

int displayHandler_get_data1() {
    return buffer[6];
}

/* The numbers of beeps that the keypad
 * will produce
 */
int displayHandler_get_beeps() {
    return buffer[6] & F7_BM_BYTE6_BEEP;
}

int displayHandler_get_data2() {
    return buffer[7];
}

/* Represents if the System is Armed Stay
 * @return true if armed in stay mode (STAY, NIGHT, INSTANT)
 */
bool displayHandler_get_armed_stay() {
    return (buffer[7] & F7_BM_BYTE7_ARMED_STAY) == F7_BM_BYTE7_ARMED_STAY ? true : false;
}

/* Represents if the system is in ready state
 * return true if system is ready (Disarmed and all zones are closed)
 */
bool displayHandler_get_ready() {
    return (buffer[7] & F7_BM_BYTE7_READY) == F7_BM_BYTE7_READY ? true : false;
}

int displayHandler_get_data3() {
    return buffer[8];
}

/* Represents if the system is in chime mode
 * will beep the keypad in any zone faults
 * return true if system is chime mode on
 */
bool displayHandler_get_chime_mode() {
    return (buffer[8] & F7_BM_BYTE8_CHIME_MODE) == F7_BM_BYTE8_CHIME_MODE ? true : false;
}

/* Represents if the system is AC powered
 * return true if system is ac powered
 */
bool displayHandler_get_ac_power() {
    return (buffer[8] & F7_BM_BYTE8_AC_POWER) == F7_BM_BYTE8_AC_POWER ? true : false;
}

/* Represents if the System is Armed Away
 * @return true if armed in away mode (AWAY, MAXIMUM)
 */
bool displayHandler_get_armed_away() {
    return (buffer[8] & F7_BM_BYTE8_ARMED_AWAY) == F7_BM_BYTE8_ARMED_AWAY ? true : false;
}

/* Represents if system is in programming mode
 * return true if in programming mode
 */
bool displayHandler_get_programming_mode() {
    return (buffer[8] & F7_BM_BYTE8_PROGRAM) == F7_BM_BYTE8_PROGRAM ? true : false;
}

/* Represents if system is displaying the prompt
 * return true if prompt displaying
 */
bool displayHandler_is_prompt_displaying() {
    return (buffer[9] == 0x01) ? true : false;
}

/* represent if the system was faulted this flag will
 * is cleared on second disarm
 * return true if system was faulted
 */
bool displayHandler_get_alarm_ocurred() {
    return (buffer[8] & F7_BM_BYTE8_FAULT) == F7_BM_BYTE8_FAULT ? true : false;
}
        
/* represent if the system alarm bell is sounding
 * is cleared onfirst disarm
 * return true if system alarm bell is sounding
 */       
bool displayHandler_get_alarm_bell() {
    return (buffer[8] & F7_BM_BYTE8_ALARM) == F7_BM_BYTE8_ALARM ? true : false;
}
        
/* represent if the entry delay is disabled (INSTANT, MAXIMUM)
 * return true if system delay is off
 */
bool displayHandler_get_entry_delay_off() {
    return (buffer[8] & F7_BM_BYTE8_ENTRY_DELAY) == F7_BM_BYTE8_ENTRY_DELAY ? true : false;
}
        
/* represent if the system is armed in perimeter only (Night)
 * return true if system perimeter only is on
 */
bool displayHandler_get_perimeter_only() {
    return (buffer[8] & F7_BM_BYTE6_NIGHT_MODE) == F7_BM_BYTE6_NIGHT_MODE ? true : false;
}

/* represent the position of the prompt in display
 * return the position of prompt in display
 */
int displayHandler_get_prompt_position() {
    return (int)buffer[10];
}

/* represents the display message on screen
 * return the string representation with non printable chars changed to @
 */
void displayHandler_get_display_message(char *displayMessage) {
        
    memset(displayMessage, 0x00, sizeof(displayMessage));

    for ( short i = 12 ; i< 44; i++ ) {
      if ( buffer[i] >= 0x20 && buffer[i] <= 0x7e ) {
        displayMessage[i-12] = buffer[i];
      } else {
        displayMessage[i-12] = 0x40;
      } 
    }
        
}

int displayHandler_get_check_sum() {
    return buffer[F7_MESSAGE_LEN-1];
}

void displayHandler_to_string(char *intBuffer) {
  char auxBuffer[35];
  char zoneBuffer[5];
  
  memset(intBuffer, 0x00, sizeof(intBuffer));

  strcat(intBuffer, "[");
  strcat(intBuffer, (displayHandler_get_ready() ? "1" : "0"));             // Ready
  strcat(intBuffer, (displayHandler_get_armed_away() ? "1" : "0"));        // Armed Away
  strcat(intBuffer, (displayHandler_get_armed_stay() ? "1" : "0"));        // Armed Stay
  strcat(intBuffer, "-" );                                  // Black Light
  strcat(intBuffer, (displayHandler_is_prompt_displaying() ? "1" : "0"));  // is promt displaying 
  sprintf(auxBuffer, "%d", displayHandler_get_beeps() );                   // Beeps
  strcat(intBuffer, auxBuffer );
  strcat(intBuffer, "-" );                                  // A ZONE OR ZONES ARE BYPASSED
  strcat(intBuffer, (displayHandler_get_ac_power()  ? "1" : "0"));         // AC Power
  strcat(intBuffer, (displayHandler_get_chime_mode() ? "1" : "0"));        // Chime Mode    
  strcat(intBuffer, (displayHandler_get_alarm_ocurred() ? "1" : "0"));     // ALARM OCCURRED STICKY BIT (cleared 2nd disarm)
  strcat(intBuffer, (displayHandler_get_alarm_bell() ? "1" : "0"));        // ALARM BELL (cleared 1st disarm)
  strcat(intBuffer, "-");                                   // BATTERY LOW TODO
  strcat(intBuffer, (displayHandler_get_entry_delay_off() ? "1" : "0"));   // ENTRY DELAY OFF (ARMED INSTANT/MAX)
  strcat(intBuffer, "-");                                   // FIRE ALARM TODO
  strcat(intBuffer, "-");                                   // CHECK ZONE TODO
  strcat(intBuffer, (displayHandler_get_perimeter_only() ? "1" : "0"));    // PERIMETER ONLY (ARMED STAY/NIGHT)
  
  strcat(intBuffer, "----],"); //23
  sprintf( zoneBuffer, "%03x,", displayHandler_get_zone_field() ); //27
  strcat (intBuffer, zoneBuffer);
  strcat(intBuffer, "\""); //28  
  displayHandler_get_display_message(auxBuffer);
  strcat(intBuffer, auxBuffer ); //60
  strcat(intBuffer, "\""); //61
  
}

void displayHandler_debug_to_string(char *intBuffer) {
  memset(intBuffer, 0x00, sizeof(intBuffer));
  strcat(intBuffer, "F7:{" );
  for ( short i = 0; i < 12; i++ ) {
      char aux[3];
      sprintf(aux, "%02x", 0xff & buffer[i] );
      strcat(intBuffer, aux );
      strcat(intBuffer, " " );
  }  
  strcat(intBuffer, "}" );
}

