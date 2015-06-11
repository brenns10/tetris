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
  tetris_block tb = {0, 0, {1, 1}};
  for (tb.typ = 0; tb.typ < NUM_TETROMINOS; tb.typ++) {
    for (tb.ori = 0; tb.ori < NUM_ORIENTATIONS; tb.ori++) {
      tg_put(tg, tb);
      tg_print(tg, stdout);
      getchar();
      printf("-------------------------\n");
      tg_remove(tg, tb);
    }
  }
  tg_print(tg, stdout);
  tg_delete(tg);
  return 0;
}
