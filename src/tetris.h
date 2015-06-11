/***************************************************************************//**

  @file         tetris.h

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Tetris game declarations.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h> // for FILE

#define TETRIS 4

#define TG_EMPTY   0x00
#define TG_FALLING 0x01
#define TG_BLOCK   0x02

#define TET_I 0
#define TET_J 1
#define TET_L 2
#define TET_O 3
#define TET_S 4
#define TET_T 5
#define TET_Z 6
#define NUM_TETROMINOS 7
#define NUM_ORIENTATIONS 4

#define TG_EMPTY_STR " "
#define TG_BLOCK_STR "\u25AE"

typedef struct {
  int row;
  int col;
} tetris_location;

typedef struct {

  int typ;
  int ori;
  tetris_location loc;

} tetris_block;

typedef struct {

  int rows;
  int cols;
  char *board;
  int points;
  int level;

} tetris_game;

extern tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS];

void tg_init(tetris_game *obj, int rows, int cols);
tetris_game *tg_create(int rows, int cols);
void tg_destroy(tetris_game *obj);
void tg_delete(tetris_game *obj);

char tg_get(tetris_game *obj, int row, int col);
void tg_set(tetris_game *obj, int row, int col, char value);
void tg_print(tetris_game *obj, FILE *f);

#endif // TETRIS_H
