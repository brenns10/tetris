/***************************************************************************//**

  @file         util.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Utilities for Tetris

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifdef WIN32
#include <windows.h> // Sleep
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>    // nanosleep
#else
#include <unistd.h>  // usleep
#endif

void sleep_milli(int milliseconds)
{
#ifdef WIN32
  Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = milleseconds * 1000 * 1000;
  nanosleep(&ts, NULL);
#else
  usleep(milliseconds * 1000);
#endif
}
