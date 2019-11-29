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

#ifndef F9E_MESSAGE_HANDLER_H
#define F9E_MESSAGE_HANDLER_H

#include "Event_Handler.h"

int const F9E_MESSAGE_LEN = 6;

class Msg9e_Handler : public Event_Handler {
    public:
        Msg9e_Handler();
        virtual int handle_event (char et);
        void reset();
        virtual void to_string(char *);
        virtual void debug_to_string(char *);
    private:
        char buffer[F9E_MESSAGE_LEN+4];
};



#endif
