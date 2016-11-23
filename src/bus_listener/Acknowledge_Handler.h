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

#ifndef F6_MESSAGE_HANDLER_H
#define F6_MESSAGE_HANDLER_H

#include "Event_Handler.h"

int const F6_MESSAGE_LEN = 2;

class Acknowledge_Handler : public Event_Handler {
    public:
        Acknowledge_Handler();
        virtual int handle_event (char et);
        void reset();
        int get_ack_address();
        virtual void to_string(char *);
        virtual void debug_to_string(char *);
    private:
        char buffer[F6_MESSAGE_LEN+2];
};



#endif
