/***************************************************************************//**

  @file         tetris.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Tetris game logic.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#include "tetris.h"

/*******************************************************************************

                               Array Definitions

*******************************************************************************/

tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS] = {
  // I
  {{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
   {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
   {{3, 0}, {3, 1}, {3, 2}, {3, 3}},
   {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
  // J
  {{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
   {{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
  // L
  {{{0, 2}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
   {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
  // O
  {{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
  // S
  {{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
   {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
   {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
  // T
  {{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
  // Z
  {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
   {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
   {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 0}}},
};

int GRAVITY_LEVEL[MAX_LEVEL+1] = {
// 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
  50, 48, 46, 44, 42, 40, 38, 36, 34, 32,
//10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  30, 28, 26, 24, 22, 20, 16, 12,  8,  4
};

/*******************************************************************************

                          Helper Functions for Blocks

*******************************************************************************/

/*
   Return the block at the given row and column.
 */
char tg_get(tetris_game *obj, int row, int column)
{
  return obj->board[obj->cols * row + column];
}

/*
  Set the block at the given row and column.
 */
static void tg_set(tetris_game *obj, int row, int column, char value)
{
  obj->board[obj->cols * row + column] = value;
}

/*
  Check whether a row and column are in bounds.
 */
bool tg_check(tetris_game *obj, int row, int col)
{
  return 0 <= row && row < obj->rows && 0 <= col && col < obj->cols;
}

/*
  Place a block onto the board.
 */
static void tg_put(tetris_game *obj, tetris_block block)
{
  int i;
  for (i = 0; i < TETRIS; i++) {
    tetris_location cell = TETROMINOS[block.typ][block.ori][i];
    tg_set(obj, block.loc.row + cell.row, block.loc.col + cell.col,
           TET_TO_BLCK(block.typ));
  }
}

/*
  Clear a block out of the board.
 */
static void tg_remove(tetris_game *obj, tetris_block block)
{
  int i;
  for (i = 0; i < TETRIS; i++) {
    tetris_location cell = TETROMINOS[block.typ][block.ori][i];
    tg_set(obj, block.loc.row + cell.row, block.loc.col + cell.col, TC_EMPTY);
  }
}

/*
  Check if a block can be placed on the board.
 */
static bool tg_fits(tetris_game *obj, tetris_block block)
{
  int i, r, c;
  for (i = 0; i < TETRIS; i++) {
    tetris_location cell = TETROMINOS[block.typ][block.ori][i];
    r = block.loc.row + cell.row;
    c = block.loc.col + cell.col;
    if (!tg_check(obj, r, c) || TC_IS_BLOCK(tg_get(obj, r, c))) {
      return false;
    }
  }
  return true;
}

/*
  Return a random tetromino type.
 */
static int random_tetromino(void) {
  return rand() % NUM_TETROMINOS;
}

/*
  Create a new falling block and populate the next falling block with a random
  one.
 */
static void tg_new_falling(tetris_game *obj)
{
  // Put in a new falling tetromino.
  obj->falling = obj->next;
  obj->next.typ = random_tetromino();
  obj->next.ori = 0;
  obj->next.loc.row = 0;
  obj->next.loc.col = obj->cols/2 - 2;
}

/*******************************************************************************

                               Game Turn Helpers

*******************************************************************************/

/*
  Tick gravity, and move the block down if gravity should act.
 */
static void tg_do_gravity_tick(tetris_game *obj)
{
  obj->ticks_till_gravity--;
  if (obj->ticks_till_gravity <= 0) {
    tg_remove(obj, obj->falling);
    obj->falling.loc.row++;
    if (tg_fits(obj, obj->falling)) {
      obj->ticks_till_gravity = GRAVITY_LEVEL[obj->level];
    } else {
      obj->falling.loc.row--;
      tg_put(obj, obj->falling);

      tg_new_falling(obj);
    }
    tg_put(obj, obj->falling);
  }
}

/*
  Move the falling tetris block left (-1) or right (+1).
 */
static void tg_move(tetris_game *obj, int direction)
{
  tg_remove(obj, obj->falling);
  obj->falling.loc.col += direction;
  if (!tg_fits(obj, obj->falling)) {
    obj->falling.loc.col -= direction;
  }
  tg_put(obj, obj->falling);
}

/*
  Send the falling tetris block to the bottom.
 */
static void tg_down(tetris_game *obj)
{
  tg_remove(obj, obj->falling);
  while (tg_fits(obj, obj->falling)) {
    obj->falling.loc.row++;
  }
  obj->falling.loc.row--;
  tg_put(obj, obj->falling);
  tg_new_falling(obj);
}

/*
  Rotate the falling block in either direction (+/-1).
 */
static void tg_rotate(tetris_game *obj, int direction)
{
  tg_remove(obj, obj->falling);

  while (true) {
    obj->falling.ori = (obj->falling.ori + direction) % NUM_ORIENTATIONS;

    // If the new orientation fits, we're done.
    if (tg_fits(obj, obj->falling))
      break;

    // Otherwise, try moving left to make it fit.
    obj->falling.loc.col--;
    if (tg_fits(obj, obj->falling))
      break;

    // Finally, try moving right to make it fit.
    obj->falling.loc.col += 2;
    if (tg_fits(obj, obj->falling))
      break;

    // Put it back in its original location and try the next orientation.
    obj->falling.loc.col--;
    // Worst case, we come back to the original orientation and it fits, so this
    // loop will terminate.
  }

  tg_put(obj, obj->falling);
}

/*
  Swap the falling block with the block in the hold buffer.
 */
static void tg_hold(tetris_game *obj)
{
  tg_remove(obj, obj->falling);
  if (obj->stored.typ == -1) {
    obj->stored = obj->falling;
    tg_new_falling(obj);
  } else {
    int typ = obj->falling.typ, ori = obj->falling.ori;
    obj->falling.typ = obj->stored.typ;
    obj->falling.ori = obj->stored.ori;
    obj->stored.typ = typ;
    obj->stored.ori = ori;
    while (!tg_fits(obj, obj->falling)) {
      obj->falling.loc.row--;
    }
  }
  tg_put(obj, obj->falling);
}

/*
  Perform the action specified by the move.
 */
static void tg_handle_move(tetris_game *obj, tetris_move move)
{
  switch (move) {
  case TM_LEFT:
    tg_move(obj, -1);
    break;
  case TM_RIGHT:
    tg_move(obj, 1);
    break;
  case TM_DROP:
    tg_down(obj);
    break;
  case TM_CLOCK:
    tg_rotate(obj, 1);
    break;
  case TM_COUNTER:
    tg_rotate(obj, -1);
    break;
  case TM_HOLD:
    tg_hold(obj);
    break;
  default:
    // pass
    break;
  }
}

/*
  Return true if line i is full.
 */
static bool tg_line_full(tetris_game *obj, int i)
{
  int j;
  for (j = 0; j < obj->cols; j++) {
    if (TC_IS_EMPTY(tg_get(obj, i, j)))
      return false;
  }
  return true;
}

/*
  Shift every row above r down one.
 */
static void tg_shift_lines(tetris_game *obj, int r)
{
  int i, j;
  for (i = r-1; i >= 0; i--) {
    for (j = 0; j < obj->cols; j++) {
      tg_set(obj, i+1, j, tg_get(obj, i, j));
      tg_set(obj, i, j, TC_EMPTY);
    }
  }
}

/*
  Find rows that are filled, remove them, shift, and return the number of
  cleared rows.
 */
static int tg_check_lines(tetris_game *obj)
{
  int i, nlines = 0;
  tg_remove(obj, obj->falling); // don't want to mess up falling block

  for (i = obj->rows-1; i >= 0; i--) {
    if (tg_line_full(obj, i)) {
      tg_shift_lines(obj, i);
      i++; // do this line over again since they're shifted
      nlines++;
    }
  }

  tg_put(obj, obj->falling); // replace
  return nlines;
}

/*
  Adjust the score for the game, given how many lines were just cleared.
 */
static void tg_adjust_score(tetris_game *obj, int lines_cleared)
{
  static int line_multiplier[] = {0, 40, 100, 300, 1200};
  obj->points += line_multiplier[lines_cleared] * (obj->level + 1);
  if (lines_cleared >= obj->lines_remaining) {
    obj->level = MIN(MAX_LEVEL, obj->level + 1);
    lines_cleared -= obj->lines_remaining;
    obj->lines_remaining = LINES_PER_LEVEL - lines_cleared;
  } else {
    obj->lines_remaining -= lines_cleared;
  }
}

/*
  Return true if the game is over.
 */
static bool tg_game_over(tetris_game *obj)
{
  int i, j;
  bool over = false;
  tg_remove(obj, obj->falling);
  for (i = 0; i < 2; i++) {
    for (j = 0; j < obj->cols; j++) {
      if (TC_IS_BLOCK(tg_get(obj, i, j))) {
        over = true;
      }
    }
  }
  tg_put(obj, obj->falling);
  return over;
}

/*******************************************************************************

                             Main Public Functions

*******************************************************************************/

/*
  Do a single game tick: process gravity, user input, and score.  Return true if
  the game is still running, false if it is over.
 */
bool tg_tick(tetris_game *obj, tetris_move move)
{
  int lines_cleared;
  // Handle gravity.
  tg_do_gravity_tick(obj);

  // Handle input.
  tg_handle_move(obj, move);

  // Check for cleared lines
  lines_cleared = tg_check_lines(obj);

  tg_adjust_score(obj, lines_cleared);

  // Return whether the game will continue (NOT whether it's over)
  return !tg_game_over(obj);
}

void tg_init(tetris_game *obj, int rows, int cols)
{
  // Initialization logic
  obj->rows = rows;
  obj->cols = cols;
  obj->board = malloc(rows * cols);
  memset(obj->board, TC_EMPTY, rows * cols);
  obj->points = 0;
  obj->level = 0;
  obj->ticks_till_gravity = GRAVITY_LEVEL[obj->level];
  obj->lines_remaining = LINES_PER_LEVEL;
  srand(time(NULL));
  tg_new_falling(obj);
  tg_new_falling(obj);
  obj->stored.typ = -1;
  obj->stored.ori = 0;
  obj->stored.loc.row = 0;
  obj->next.loc.col = obj->cols/2 - 2;
  printf("%d", obj->falling.loc.col);
}

tetris_game *tg_create(int rows, int cols)
{
  tetris_game *obj = malloc(sizeof(tetris_game));
  tg_init(obj, rows, cols);
  return obj;
}

void tg_destroy(tetris_game *obj)
{
  // Cleanup logic
  free(obj->board);
}

void tg_delete(tetris_game *obj) {
  tg_destroy(obj);
  free(obj);
}

/*
  Load a game from a file.
 */
tetris_game *tg_load(FILE *f)
{
  tetris_game *obj = malloc(sizeof(tetris_game));
  fread(obj, sizeof(tetris_game), 1, f);
  obj->board = malloc(obj->rows * obj->cols);
  fread(obj->board, sizeof(char), obj->rows * obj->cols, f);
  return obj;
}

/*
  Save a game to a file.
 */
void tg_save(tetris_game *obj, FILE *f)
{
  fwrite(obj, sizeof(tetris_game), 1, f);
  fwrite(obj->board, sizeof(char), obj->rows * obj->cols, f);
}

/*
  Print a game board to a file.  Really just for early debugging.
 */
void tg_print(tetris_game *obj, FILE *f) {
  int i, j;
  for (i = 0; i < obj->rows; i++) {
    for (j = 0; j < obj->cols; j++) {
      if (TC_IS_EMPTY(tg_get(obj, i, j))) {
        fputs(TC_EMPTY_STR, f);
      } else {
        fputs(TC_BLOCK_STR, f);
      }
    }
    fputc('\n', f);
  }
}
