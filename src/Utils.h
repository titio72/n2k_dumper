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

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <math.h>

int getDaysSince1970(int y, int m, int d);
char* replace(char const * const original, char const * const pattern, char const * const replacement, bool first = false);
bool array_contains(int test, int* int_set, int sz);
ulong _millis();
int msleep(long msec);

void format_thousands_sep(char* buffer, long l);

inline double to_degrees(double radians) {
    return radians * 180.0 / M_PI;
}

inline double to_radians(double degrees) {
    return degrees / 180.0 * M_PI;
}

inline double to_meters_per_second(double kn) {
    return kn / 3.6 * 1.852;
}

inline double to_knots(double ms) {
    return ms * 3.6 / 1.852;
}

#endif
