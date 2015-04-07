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
#ifndef F7_MESSAGE_HANDLER_H
#define F7_MESSAGE_HANDLER_H

#include "Event_Handler.h"

/*********************************************
 *       STRUCT TO HANDLE F7 MESSAGES
 *********************************************/
// F7 Message
int const F7_MESSAGE_LEN = 45;

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

class Display_Handler : public Event_Handler {
    public:
        Display_Handler();
        virtual int handle_event (char et);
        void reset();
        int get_addr1();
        int get_addr2();
        int get_addr3();
        int get_addr4();
        int get_zone_field();
        int get_data1();
        int get_beeps();
        int get_data2();
        boolean get_armed_stay();
        boolean get_ready();
        int get_data3();
        boolean get_chime_mode();
        boolean get_ac_power();
        boolean get_armed_away();
        boolean get_programming_mode();
        boolean is_prompt_displaying();
        boolean get_alarm_ocurred();
        boolean get_alarm_bell();
        boolean get_entry_delay_off();
        boolean get_perimeter_only();        
        int get_prompt_position();
        char* get_display_message();
        int get_check_sum();
        virtual void to_string(char *);
    private:
        char buffer[F7_MESSAGE_LEN+1];        
};



#endif
