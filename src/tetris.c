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

void tg_init(tetris_game *obj, int rows, int cols)
{
  // Initialization logic
  obj->rows = rows;
  obj->cols = cols;
  obj->board = malloc(rows * cols);
  memset(obj->board, TG_EMPTY, rows * cols);
  obj->points = 0;
  obj->level = 1;
  obj->ticks_till_gravity = TICKS_PER_GRAVITY;
  srand(time(NULL));
  obj->falling.typ = random_tetromino();
  obj->falling.ori = 0;
  obj->falling.loc.row = 0;
  obj->falling.loc.col = 0;
  obj->next.typ = random_tetromino();
  obj->next.ori = 0;
  obj->next.loc.row = 0;
  obj->next.loc.col = 0;
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

char tg_get(tetris_game *obj, int row, int column)
{
  return obj->board[obj->cols * row + column];
}

void tg_set(tetris_game *obj, int row, int column, char value)
{
  obj->board[obj->cols * row + column] = value;
}

bool tg_check(tetris_game *obj, int row, int col)
{
  return 0 <= row && row < obj->rows && 0 <= col && col < obj->cols;
}

void tg_put(tetris_game *obj, tetris_block block)
{
  int i;
  for (i = 0; i < TETRIS; i++) {
    tetris_location cell = TETROMINOS[block.typ][block.ori][i];
    tg_set(obj, block.loc.row + cell.row, block.loc.col + cell.col,
           TET_TO_BLCK(block.typ));
  }
}

void tg_remove(tetris_game *obj, tetris_block block)
{
  int i;
  for (i = 0; i < TETRIS; i++) {
    tetris_location cell = TETROMINOS[block.typ][block.ori][i];
    tg_set(obj, block.loc.row + cell.row, block.loc.col + cell.col, TG_EMPTY);
  }
}

bool tg_fits(tetris_game *obj, tetris_block block)
{
  int i, r, c;
  for (i = 0; i < TETRIS; i++) {
    tetris_location cell = TETROMINOS[block.typ][block.ori][i];
    r = block.loc.row + cell.row;
    c = block.loc.col + cell.col;
    if (!tg_check(obj, r, c) || TG_IS_BLOCK(tg_get(obj, r, c))) {
      return false;
    }
  }
  return true;
}

static void tg_new_falling(tetris_game *obj)
{
  // Put in a new falling tetromino.
  obj->falling = obj->next;
  obj->next.typ = random_tetromino();
  obj->next.ori = 0;
  obj->next.loc.row = 0;
  obj->next.loc.col = 0;
  // TODO - if new block fails, game over.
}

static void tg_do_gravity_tick(tetris_game *obj)
{
  obj->ticks_till_gravity--;
  if (obj->ticks_till_gravity <= 0) {
    tg_remove(obj, obj->falling);
    obj->falling.loc.row++;
    if (tg_fits(obj, obj->falling)) {
      obj->ticks_till_gravity = TICKS_PER_GRAVITY;
    } else {
      obj->falling.loc.row--;
      tg_put(obj, obj->falling);

      tg_new_falling(obj);
    }
    tg_put(obj, obj->falling);
  }
}

static void tg_move(tetris_game *obj, int direction)
{
  tg_remove(obj, obj->falling);
  obj->falling.loc.col += direction;
  if (!tg_fits(obj, obj->falling)) {
    obj->falling.loc.col -= direction;
  }
  tg_put(obj, obj->falling);
}

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
  default:
    // pass
    break;
  }
}

static bool tg_line_full(tetris_game *obj, int i)
{
  int j;
  for (j = 0; j < obj->cols; j++) {
    if (TG_IS_EMPTY(tg_get(obj, i, j)))
      return false;
  }
  return true;
}

static void tg_shift_lines(tetris_game *obj, int r)
{
  int i, j;
  for (i = r-1; i >= 0; i--) {
    for (j = 0; j < obj->cols; j++) {
      tg_set(obj, i+1, j, tg_get(obj, i, j));
      tg_set(obj, i, j, TG_EMPTY);
    }
  }
}

static void tg_check_lines(tetris_game *obj)
{
  int i;
  tg_remove(obj, obj->falling); // don't want to mess up falling block

  for (i = obj->rows-1; i >= 0; i--) {
    if (tg_line_full(obj, i)) {
      tg_shift_lines(obj, i);
      obj->points++;
      i++; // do this line over again since they're shifted
    }
  }

  tg_put(obj, obj->falling); // replace
}

void tg_tick(tetris_game *obj, tetris_move move)
{
  // Handle gravity.
  tg_do_gravity_tick(obj);

  // Handle input.
  tg_handle_move(obj, move);

  // Check for cleared lines
  tg_check_lines(obj);
}

void tg_print(tetris_game *obj, FILE *f) {
  int i, j;
  for (i = 0; i < obj->rows; i++) {
    for (j = 0; j < obj->cols; j++) {
      if (TG_IS_EMPTY(tg_get(obj, i, j))) {
        fputs(TG_EMPTY_STR, f);
      } else {
        fputs(TG_BLOCK_STR, f);
      }
    }
    fputc('\n', f);
  }
}

void tg_curses(tetris_game *obj)
{
  int i, j, b;

  // Top border and border around next block.
  move(0,0);
  addch(ACS_ULCORNER);
  for (i = 0; i < obj->cols; i++)
    addch(ACS_HLINE);
  addch(ACS_TTEE);
  for (i = 0; i < TETRIS; i++)
    addch(ACS_HLINE);
  addch(ACS_URCORNER);
  addch('\n');

  // Game board area
  for (i = 0; i < obj->rows; i++) {
    // left border
    addch(ACS_VLINE);

    // game cells
    for (j = 0; j < obj->cols; j++) {
      if (TG_IS_EMPTY(tg_get(obj, i, j))) {
        addch(TG_EMPTY_CURS);
      } else {
        addch(TG_BLOCK_CURS(tg_get(obj, i, j)));
      }
    }

    // to the right of the game:
    if (i < 4) {
      // fill in the next block space
      addch(ACS_VLINE);
      for (j = 0; j < TETRIS; j++) {
        bool displayed = false;
        for (b = 0; b < TETRIS; b++) {
          tetris_location c = TETROMINOS[obj->next.typ][obj->next.ori][b];
          if (c.row == i && c.col == j) {
            addch(TG_BLOCK_CURS(TET_TO_BLCK(obj->next.typ)));
            displayed = true;
          }
        }
        if (!displayed)
          addch(TG_EMPTY_CURS);
      }
      // right border
      addch(ACS_VLINE);
    } else if (i == 4) {
      // fill border under next block space
      addch(ACS_LTEE);
      for (j = 0; j < TETRIS; j++) {
        addch(ACS_HLINE);
      }
      addch(ACS_LRCORNER);
    } else if (i == 5) {
      addch(ACS_VLINE);
      printw("Score", obj->points);
    } else if (i == 6) {
      addch(ACS_VLINE);
      printw("%d", obj->points);
    } else {
      // just put the right border in
      addch(ACS_VLINE);
    }
    addch('\n');
  }

  // Lower border
  addch(ACS_LLCORNER);
  for (i = 0; i < obj->cols; i++)
    addch(ACS_HLINE);
  addch(ACS_LRCORNER);
  refresh();
}

void tg_init_colors(void)
{
  start_color();
  //init_color(COLOR_ORANGE, 1000, 647, 0);
  init_pair(TG_BLCKI, COLOR_CYAN, COLOR_BLACK);
  init_pair(TG_BLCKJ, COLOR_BLUE, COLOR_BLACK);
  init_pair(TG_BLCKL, COLOR_WHITE, COLOR_BLACK);
  init_pair(TG_BLCKO, COLOR_YELLOW, COLOR_BLACK);
  init_pair(TG_BLCKS, COLOR_GREEN, COLOR_BLACK);
  init_pair(TG_BLCKT, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(TG_BLCKZ, COLOR_RED, COLOR_BLACK);
}

int random_tetromino(void) {
  return rand() % NUM_TETROMINOS;
}
