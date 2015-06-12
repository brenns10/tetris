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
#include <stdbool.h> // for bool

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#define TETRIS 4

#define TG_EMPTY   0
#define TG_BLCKI   1
#define TG_BLCKJ   2
#define TG_BLCKL   3
#define TG_BLCKO   4
#define TG_BLCKS   5
#define TG_BLCKT   6
#define TG_BLCKZ   7

#define TET_TO_BLCK(x) ((x)+1)

#define TG_IS_EMPTY(x) ((x) == TG_EMPTY)
#define TG_IS_BLOCK(x) (!TG_IS_EMPTY(x))

#define TET_I 0
#define TET_J 1
#define TET_L 2
#define TET_O 3
#define TET_S 4
#define TET_T 5
#define TET_Z 6
#define NUM_TETROMINOS 7
#define NUM_ORIENTATIONS 4

#define CELLS_PER_BLOCK 2
#define TG_EMPTY_STR " "
#define TG_BLOCK_STR "\u2588"
#define TG_EMPTY_CURS(w) waddch(w,' '); waddch(w,' ')
#define TG_BLOCK_CURS(w,x) waddch((w),' '|A_REVERSE|COLOR_PAIR(x));     \
                           waddch((w),' '|A_REVERSE|COLOR_PAIR(x))

#define MAX_LEVEL 19
#define LINES_PER_LEVEL 10

typedef struct {
  int row;
  int col;
} tetris_location;

typedef struct {

  int typ;
  int ori;
  tetris_location loc;

} tetris_block;

typedef enum {
  TM_LEFT, TM_RIGHT, TM_CLOCK, TM_COUNTER, TM_DROP, TM_HOLD, TM_NONE
} tetris_move;

typedef struct {

  int rows;
  int cols;
  char *board;
  int points;
  int level;
  tetris_block falling;
  tetris_block next;
  tetris_block stored;
  int ticks_till_gravity;
  int lines_remaining;

} tetris_game;

extern tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS];
extern int GRAVITY_LEVEL[MAX_LEVEL+1];

// Data structure manipulation.
void tg_init(tetris_game *obj, int rows, int cols);
tetris_game *tg_create(int rows, int cols);
void tg_destroy(tetris_game *obj);
void tg_delete(tetris_game *obj);
tetris_game *tg_load(FILE *f);
void tg_save(tetris_game *obj, FILE *f);

// Low level cell manipulation.
char tg_get(tetris_game *obj, int row, int col);
void tg_set(tetris_game *obj, int row, int col, char value);

// Tetromino level manipulation.
void tg_put(tetris_game *obj, tetris_block block);
void tg_remove(tetris_game *obj, tetris_block block);
bool tg_tick(tetris_game *obj, tetris_move move);

// printing stuff
void tg_print(tetris_game *obj, FILE *f);
void tg_curses(tetris_game *obj);
void tg_init_colors(void);

// Other stuff
int random_tetromino(void);
void tg_init_colors(void);

#endif // TETRIS_H
