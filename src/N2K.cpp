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

#define SOCKET_CAN_PORT "vcan0"
//#define NODE_ONLY

#include <NMEA2000_CAN.h>
#include <time.h>
#include <math.h>
#include "N2K.h"
#include "Utils.h"
#include "Log.h"

void (*_handler)(const tN2kMsg &N2kMsg);

void private_message_handler(const tN2kMsg &N2kMsg) {
    _handler(N2kMsg);
}

void N2K::loop() {
    NMEA2000.ParseMessages();
}

bool N2K::sendMessage(int dest, ulong pgn, int priority, int len, unsigned char* payload) {
    tN2kMsg m(src);
    m.Init(priority, pgn, src, dest);
    for (int i = 0; i<len; i++) m.AddByte(payload[i]);
    return send_msg(m);
}

void N2K::setup(void (*_MsgHandler)(const tN2kMsg &N2kMsg), uint8_t _src) {
    src = _src;
    _handler = _MsgHandler;
    Log::trace("Initializing N2K\n");
    NMEA2000.SetN2kCANSendFrameBufSize(3);
    NMEA2000.SetN2kCANReceiveFrameBufSize(150),
    NMEA2000.SetN2kCANMsgBufSize(15);
    Log::trace("Initializing N2K Product Info\n");
    NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                /*1234567890123456789012345678901234567890*/
                                 "ABN2k                           ",  // Manufacturer's Model ID
                                 "1.0.2.25 (2019-07-07)",  // Manufacturer's Software version code
                                 "1.0.2.0 (2019-07-07)" // Manufacturer's Model version
                                 );
    Log::trace("Initializing N2K Device Info\n");
    NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                145, // Device function=Analog to NMEA 2000 Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                60, // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
    Log::trace("Initializing N2K mode\n");
    #ifdef NODE_ONLY
    NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly, src);
    #else
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, src);
    NMEA2000.SetMsgHandler(private_message_handler);
    #endif
    NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
    Log::trace("Initializing N2K Port & Handlers\n");
    bool initialized = NMEA2000.Open();
    Log::trace("Initializing N2K %s\n", initialized?"OK":"KO");

}

bool N2K::send_msg(const tN2kMsg &N2kMsg) {
    _handler(N2kMsg);
    if (NMEA2000.SendMsg(N2kMsg)) {
        return true;
    } else {
        Log::trace("Failed message {%d}\n", N2kMsg.PGN);
        return false;
    }
}
