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
#ifndef F2_MESSAGE_HANDLER_H
#define F2_MESSAGE_HANDLER_H

#include "Event_Handler.h"

// F2 Message
int const F2_MAX_MESSAGE_LEN = 128;

int const F2_BYTE1_STATE_DISARMED = 0x01;
int const F2_BYTE1_STATE_ARMED = 0x02;
int const F2_BYTE1_STATE_CHIME = 0x03;
int const F2_BYTE1_STATE_BUSY = 0x04;

int const F2_BYTE2_ARMED_MODE_STAY = 0x01;
int const F2_BYTE2_ARMED_MODE_AWAY = 0x02;


int const F2_BYTE4_FAULT_NORMAL = 0x01;
int const F2_BYTE4_FAULT_COUNTING_EXIT = 0x02;
int const F2_BYTE4_FAULT_COUNTING_ENTER = 0x03;
int const F2_BYTE4_FAULT_ALARM = 0x04;
int const F2_BYTE4_FAULT_BYPASSED = 0x06;
int const F2_BYTE4_FAULT_PANIC = 0x08;


class Status_Handler : public Event_Handler {
    public:
        Status_Handler();
        virtual int handle_event (char et);
        void reset();
        void set_read_size(int readSizePa);
        int get_read_size();
        int parse();
        int get_counter();
        int get_partition();
        int get_state();
        void get_state_str(char *);
        int get_armed_mode();
        void get_armed_mode_str(char *);
        int get_by_pass();
        int get_fault();
        void get_fault_str(char *); 
        int get_count_down();
        virtual void to_string(char *);
        virtual void debug_to_string(char *);
    private:
        char buffer[F2_MAX_MESSAGE_LEN+1];
        int readSize;
        int startPosition;
};



#endif
