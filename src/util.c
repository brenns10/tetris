/***************************************************************************//**

  @file         util.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Utilities for Tetris

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#define _POSIX_C_SOURCE 199309L

#include <time.h>    // nanosleep


void sleep_milli(int milliseconds)
{
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = milliseconds * 1000 * 1000;
  nanosleep(&ts, NULL);
}
