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

#include "Utils.h"
#include "errno.h"
#include <time.h>
#include <math.h>
#include <string.h>
#include <cstdio>

ulong _millis(void)
{
  long            ms; // Milliseconds
  time_t          s;  // Seconds
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);

  s  = spec.tv_sec;
  ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
  if (ms > 999) {
      s++;
      ms = 0;
  }

  return s * 1000 + ms;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void format_thousands_sep(char* final, long toBeFormatted)
{
    // Get the string representation as is
    static char buffer[32];
    sprintf(buffer, "%ld", toBeFormatted);

    // Calculate how much commas there will be
    unsigned int buff_length = strlen(buffer);
    unsigned int num_commas = buff_length / 3;
    unsigned int digits_left = buff_length % 3;
    if (digits_left == 0)
    {
        num_commas--;
    }

    // Allocate space for final string representation
    unsigned int final_length = buff_length + num_commas + 1;

    // Parse strings from last to first to count positions
    int final_pos = final_length - 2;
    int buff_pos = buff_length - 1;
    int i = 0;
    int commas = 0;
    while(final_pos >= 0)
    {
        final[final_pos--] = buffer[buff_pos--];
        i++;
        if (i % 3 == 0 && commas < num_commas)
        {
          commas++;
          final[final_pos--] = ',';
        }
    }
    final[final_length - 1] = 0;
}


char * replace(char const * const original, char const * const pattern, char const * const replacement, bool first) {
  size_t const replen = strlen(replacement);
  size_t const patlen = strlen(pattern);
  size_t const orilen = strlen(original);

  size_t patcnt = 0;
  const char * oriptr;
  const char * patloc;

  if (first) {
    patcnt += strstr(original, pattern)?1:0;
  } else {
    for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen) {
      patcnt++;
    }
  }

  size_t const retlen = orilen + patcnt * (replen - patlen);
  char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

  if (returned != NULL)
  {
    char * retptr = returned;
    for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen)
    {
      size_t const skplen = patloc - oriptr;
      strncpy(retptr, oriptr, skplen);
      retptr += skplen;
      strncpy(retptr, replacement, replen);
      retptr += replen;
    }
    strcpy(retptr, oriptr);

    return returned;
  } else {
    return NULL;
  }
}

const int monthDays[12] = {31, 28, 31, 30, 31, 30,
                           31, 31, 30, 31, 30, 31};

int countLeapYears(int year, int month)
{
    // Check if the current year needs to be considered
    // for the count of leap years or not
    if (month <= 2)
        year--;

    // An year is a leap year if it is a multiple of 4,
    // multiple of 400 and not a multiple of 100.
    return year / 4 - year / 100 + year / 400;
}

int getDaysSince1970(int y, int m, int d) {
    // COUNT TOTAL NUMBER OF DAYS BEFORE FIRST DATE 'dt1'

    // initialize count using years and day
    long int n1 = 1970 * 365 + 1;

    // Since every leap year is of 366 days,
    // Add a day for every leap year
    n1 += countLeapYears(1970, 1);

    // SIMILARLY, COUNT TOTAL NUMBER OF DAYS BEFORE 'dt2'

    long int n2 = y * 365 + d;
    for (int i = 0; i < m - 1; i++)
        n2 += monthDays[i];
    n2 += countLeapYears(y, m);

    // return difference between two counts
    return (n2 - n1);
}

bool array_contains(int test, int* int_set, int sz) {
    for (int i=0; i<sz; i++) {
        if (test==int_set[i]) return true;
    }
    return false;
}