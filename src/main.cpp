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

#include "N2K.h"
#include "Utils.h"
#include "Log.h"
#include "Log.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _DO_DUMP

N2K n2k;
time_t last_gps_time = 0;
unsigned long last_stw_time = 0;
unsigned long last_sent_stw_time = 0;

void time_to_ISO(time_t t, char *buf)
{
  strftime(buf, sizeof "2011-10-08T07:07:09Z", "%FT%TZ", gmtime(&t));
}

FILE *get_file(time_t &now)
{
  if (now == 0)
    return NULL;
  static int last_h = -1;
  static FILE *current_file = NULL;
  struct tm *ptm;
  ptm = gmtime(&now);
  int hour = ptm->tm_hour;
  if (last_h != hour)
  {
    static char log_name[32];
    strftime(log_name, sizeof "2011-10-08T07Z.log", "%FT%HZ.log", ptm);
    if (current_file)
    {
      fclose(current_file);
    }
    current_file = fopen(log_name, "a");
    last_h = hour;
  }

  return current_file;
}

void do_log(const char *str)
{
  time_t now = time(0);
  static char time_buf[32];
  time_to_ISO(now, time_buf);

  FILE *f = get_file(last_gps_time);

  if (f)
  {
    fprintf(f, "%s %s\n", (const char *)time_buf, str);
    fflush(f);
  }
}

void handleSOG2STW(unsigned long now, double sog)
{
  if ((now - last_sent_stw_time) > 950 // don't send more than 1 msg per second
      && (now - last_stw_time) > 15000 // send only if the STW has not been available for more than 15 secs
  )
  {
    last_sent_stw_time = now;
    tN2kMsg stwMsg(21, 2, 128259);
    stwMsg.AddByte(1);
    stwMsg.Add2ByteDouble(sog, 0.01);
    stwMsg.Add2ByteDouble(N2kDoubleNA, 0.01);
    stwMsg.AddByte(0); // Type: paddle wheel
    stwMsg.AddByte(0);
    stwMsg.AddByte(0);
    n2k.send_msg(stwMsg);
  }
}

void msg_handler(const tN2kMsg &N2kMsg)
{
  int pgn = N2kMsg.PGN;

  static char buffer[256];
  buffer[0] = 0;

  switch (pgn)
  {
  case 128259:
  {
    if (N2kMsg.Source != 21)
      last_stw_time = millis();
  }
  break;
  case 129026:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    unsigned int ref = N2kMsg.GetByte(Index) & 0b11000000;
    double cog = N2kMsg.Get2ByteDouble(0.0001, Index);
    double sog = N2kMsg.Get2ByteDouble(0.01, Index);
    handleSOG2STW(millis(), sog);
  }
  break;
  #ifdef _DO_DUMP
  case 129029:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    uint16_t DaysSince1970 = N2kMsg.Get2ByteUInt(Index);
    double SecondsSinceMidnight = N2kMsg.Get4ByteUDouble(0.0001, Index);
    time_t t = DaysSince1970 * 24 * 60 * 60 + (int)SecondsSinceMidnight;
    last_gps_time = t;
    static char buf[32];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&t));
    double Latitude = N2kMsg.Get8ByteDouble(1e-16, Index);
    double Longitude = N2kMsg.Get8ByteDouble(1e-16, Index);
    sprintf(buffer, "GNSS %12.7f %12.7f %s", Longitude, Latitude, buf);
  }
  break;
  case 130306:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    double WindSpeed = N2kMsg.Get2ByteUDouble(0.01, Index);
    double WindAngle = N2kMsg.Get2ByteUDouble(0.0001, Index);
    tN2kWindReference WindReference = (tN2kWindReference)(N2kMsg.GetByte(Index) & 0x07);
    if (WindReference == tN2kWindReference::N2kWind_Apparent)
    {
      sprintf(buffer, "Wind %5.2f %5.1f", to_knots(WindSpeed), to_degrees(WindAngle));
    }
  }
  break;
  case 127257:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    double Yaw = N2kMsg.Get2ByteDouble(0.0001, Index);
    double Pitch = N2kMsg.Get2ByteDouble(0.0001, Index);
    double Roll = N2kMsg.Get2ByteDouble(0.0001, Index);
    sprintf(buffer, "Roll %5.1f %5.1f", to_degrees(Roll), to_degrees(Pitch));
  }
  break;
  case 130312:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    unsigned int TempInstance = N2kMsg.GetByte(Index);
    tN2kTempSource TempSource = (tN2kTempSource)(N2kMsg.GetByte(Index));
    double ActualTemperature = N2kMsg.Get2ByteUDouble(0.01, Index);
    double SetTemperature = N2kMsg.Get2ByteUDouble(0.01, Index);
    switch (TempSource)
    {
    case tN2kTempSource::N2kts_MainCabinTemperature:
    {
      sprintf(buffer, "CabT %d %5.2f", TempInstance, ActualTemperature - 273.15);
    }
    break;
    case tN2kTempSource::N2kts_SeaTemperature:
    {
      sprintf(buffer, "SeaT %d %5.2f", TempInstance, ActualTemperature - 273.15);
    }
    break;
    default:
      break;
    }
  }
  break;
  case 130314:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    unsigned int PressureInstance = N2kMsg.GetByte(Index);
    double PressureSource = (tN2kPressureSource)N2kMsg.GetByte(Index);
    double ActualPressure = N2kMsg.Get4ByteDouble(0.1, Index);
    if (PressureSource == tN2kPressureSource::N2kps_Atmospheric)
    {
      sprintf(buffer, "Pres %d %6.1f", PressureInstance, PascalTohPA(ActualPressure));
    }
  }
  break;
  case 130313:
  {
    int Index = 0;
    unsigned char SID = N2kMsg.GetByte(Index);
    unsigned int HumidityInstance = N2kMsg.GetByte(Index);
    tN2kHumiditySource HumiditySource = (tN2kHumiditySource)N2kMsg.GetByte(Index);
    double ActualHumidity = N2kMsg.Get2ByteDouble(0.004, Index);
    double SetHumidity = N2kMsg.Get2ByteDouble(0.004, Index);
    if (HumiditySource == tN2kHumiditySource::N2khs_InsideHumidity)
    {
      sprintf(buffer, "HumI %d %4.1f", HumidityInstance, ActualHumidity);
    }
  }
  break;
  #endif
  default:
    break;
  }
  if (buffer[0])
  {
    do_log(buffer);
  }
}

void setup()
{
  n2k.setup(msg_handler, 21);
}

void loop()
{
  static ulong t0 = _millis();
  ulong t = _millis();
  if ((t - t0) < 25)
  {
    msleep(25);
  }
  t0 = t;

  n2k.loop();
}

int main(int argc, const char **argv)
{
  setup();
  while (1)
  {
    loop();
  }
}