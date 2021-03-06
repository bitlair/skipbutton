/*
 * boblight
 * Copyright (C) Bob  2009 
 * 
 * boblight is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * boblight is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEUTILS
#define TIMEUTILS

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <time.h>

inline int64_t GetTimeUs()
{
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);

  return ((int64_t)time.tv_sec * 1000000LL) + (int64_t)(time.tv_nsec + 500) / 1000LL;
}

template <class T> 
inline T GetTimeSec()
{
  return (T)GetTimeUs() / (T)1000000.0;
}

void USleep(int64_t usecs, volatile bool* stop = NULL);

#endif //TIMEUTILS
