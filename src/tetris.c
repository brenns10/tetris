/***************************************************************************//**

  @file         tetris.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Tetris game logic.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "tetris.h"

void tg_init(tetris_game *obj, int rows, int cols)
{
  // Initialization logic
  obj->rows = rows;
  obj->cols = cols;
  obj->board = smb_new(char, rows * cols);
  obj->points = 0;
  obj->level = 1;
}

tetris_game *tg_create(int rows, int cols)
{
  tetris_game *obj = smb_new(tetris_game, 1);
  tg_init(obj, rows, cols);
  return obj;
}

void tg_destroy(tetris_game *obj)
{
  // Cleanup logic
  smb_free(obj->board);
}

void tg_delete(tetris_game *obj) {
  tg_destroy(obj);
  smb_free(obj);
}

