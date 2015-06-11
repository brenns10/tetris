/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Main program for tetris.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>

#include "tetris.h"

int main(int argc, char *argv[])
{
  tetris_game *tg = tg_create(10, 10);
  tg_print(tg, stdout);
  tg_set(tg, 5, 5, TG_BLOCK);
  tg_print(tg, stdout);
  tg_delete(tg);
  return 0;
}
