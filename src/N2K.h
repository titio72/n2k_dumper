/*
(C) 2023, Andrea Boni
This file is part of n2k_dumper.
n2k_dumper is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
n2k_dumper is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with n2k_dumper.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef N2K_H
#define N2K_H

#include <N2kMessages.h>
#include "Utils.h"

class N2K {

    public:

        bool sendMessage(int dest, ulong pgn, int priority, int len, unsigned char* payload);

        void setup(void (*_MsgHandler)(const tN2kMsg &N2kMsg), uint8_t src);

        void loop();

        bool send_msg(const tN2kMsg &N2kMsg);

    private:
        uint8_t src;

};

#endif