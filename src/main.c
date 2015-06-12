/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Main program for tetris.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#include "tetris.h"
#include "util.h"

/*
  2 columns per cell makes the game much nicer.
 */
#define COLS_PER_CELL 2
/*
  Macro to print a cell of a specific type to a window.
 */
#define ADD_BLOCK(w,x) waddch((w),' '|A_REVERSE|COLOR_PAIR(x));     \
                       waddch((w),' '|A_REVERSE|COLOR_PAIR(x))

/*
  Print the tetris board onto the ncurses window.
 */
void display_board(WINDOW *w, tetris_game *obj)
{
  int i, j;
  wclear(w);
  box(w, 0, 0);
  wmove(w,1,1);
  for (i = 0; i < obj->rows; i++) {
    for (j = 0; j < obj->cols; j++) {
      if (TC_IS_FILLED(tg_get(obj, i, j))) {
        wmove(w, 1 + i, 1 + j*COLS_PER_CELL);
        ADD_BLOCK(w,tg_get(obj, i, j));
      }
    }
  }
  wrefresh(w);
}

/*
  Display a tetris piece in a dedicated window.
*/
void display_piece(WINDOW *w, tetris_block block)
{
  int b;
  tetris_location c;
  wclear(w);
  box(w, 0, 0);
  if (block.typ == -1) {
    wrefresh(w);
    return;
  }
  for (b = 0; b < TETRIS; b++) {
    c = TETROMINOS[block.typ][block.ori][b];
    wmove(w, c.row + 1, c.col * COLS_PER_CELL + 1);
    ADD_BLOCK(w, TYPE_TO_CELL(block.typ));
  }
  wrefresh(w);
}

/*
  Display score information in a dedicated window.
 */
void display_score(WINDOW *w, tetris_game *tg)
{
  wclear(w);
  box(w, 0, 0);
  wprintw(w, "Score\n%d\n", tg->points);
  wprintw(w, "Level\n%d\n", tg->level);
  wprintw(w, "Lines\n%d\n", tg->lines_remaining);
  wrefresh(w);
}

/*
  Boss mode!  Make it look like you're doing work.
 */
void boss_mode(void)
{
  clear();
  printw("user@workstation-312:~/Documents/presentation $ ls -l\n"
         "total 528\n"
         "drwxr-xr-x 2 user users   4096 Jun  9 17:05 .\n"
         "drwxr-xr-x 4 user users   4096 Jun 10 09:52 ..\n"
         "-rw-r--r-- 1 user users  88583 Jun  9 14:13 figure1.png\n"
         "-rw-r--r-- 1 user users  65357 Jun  9 15:40 figure2.png\n"
         "-rw-r--r-- 1 user users   4469 Jun  9 16:17 presentation.aux\n"
         "-rw-r--r-- 1 user users  42858 Jun  9 16:17 presentation.log\n"
         "-rw-r--r-- 1 user users   2516 Jun  9 16:17 presentation.nav\n"
         "-rw-r--r-- 1 user users    183 Jun  9 16:17 presentation.out\n"
         "-rw-r--r-- 1 user users 349607 Jun  9 16:17 presentation.pdf\n"
         "-rw-r--r-- 1 user users      0 Jun  9 16:17 presentation.snm\n"
         "-rw-r--r-- 1 user users   9284 Jun  9 17:05 presentation.tex\n"
         "-rw-r--r-- 1 user users    229 Jun  9 16:17 presentation.toc\n"
         "\n"
         "user@workstation-312:~/Documents/presentation $ ");
  echo();
  timeout(-1);
  while (getch() != KEY_F(1));
  timeout(0);
  noecho();
  clear();
}

/*
  Save and exit the game.
 */
void save(tetris_game *game, WINDOW *w)
{
  FILE *f;

  wclear(w);
  wprintw(w, "Save and exit? [Y/n] ");
  wrefresh(w);
  timeout(-1);
  if (getch() == 'n') {
    timeout(0);
    return;
  }
  f = fopen("tetris.save", "w");
  tg_save(game, f);
  fclose(f);
  tg_delete(game);
  endwin();
  printf("Game saved to \"tetris.save\".\n");
  printf("Resume by passing the filename as an argument to this program.\n");
  exit(EXIT_SUCCESS);
}

/*
  Do the NCURSES initialization steps for color blocks.
 */
void init_colors(void)
{
  start_color();
  //init_color(COLOR_ORANGE, 1000, 647, 0);
  init_pair(TC_CELLI, COLOR_CYAN, COLOR_BLACK);
  init_pair(TC_CELLJ, COLOR_BLUE, COLOR_BLACK);
  init_pair(TC_CELLL, COLOR_WHITE, COLOR_BLACK);
  init_pair(TC_CELLO, COLOR_YELLOW, COLOR_BLACK);
  init_pair(TC_CELLS, COLOR_GREEN, COLOR_BLACK);
  init_pair(TC_CELLT, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(TC_CELLZ, COLOR_RED, COLOR_BLACK);
}

/*
  Main tetris game!
 */
int main(int argc, char **argv)
{
  tetris_game *tg;
  tetris_move move = TM_NONE;
  bool running = true;
  WINDOW *board, *next, *hold, *score;

  // Load file if given a filename.
  if (argc >= 2) {
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
      perror("tetris");
      exit(EXIT_FAILURE);
    }
    tg = tg_load(f);
    fclose(f);
  } else {
    // Otherwise create new game.
    tg = tg_create(22, 10);
  }

  // NCURSES initialization:
  initscr();             // initialize curses
  cbreak();              // pass key presses to program, but not signals
  noecho();              // don't echo key presses to screen
  keypad(stdscr, TRUE);  // allow arrow keys
  timeout(0);            // no blocking on getch()
  curs_set(0);           // set the cursor to invisible
  init_colors();         // setup tetris colors

  // Create windows for each section of the interface.
  board = newwin(tg->rows + 2, 2 * tg->cols + 2, 0, 0);
  next  = newwin(6, 10, 0, 2 * (tg->cols + 1) + 1);
  hold  = newwin(6, 10, 7, 2 * (tg->cols + 1) + 1);
  score = newwin(6, 10, 14, 2 * (tg->cols + 1 ) + 1);

  // Game loop
  while (running) {
    running = tg_tick(tg, move);
    display_board(board, tg);
    display_piece(next, tg->next);
    display_piece(hold, tg->stored);
    display_score(score, tg);
    sleep_milli(10);

    switch (getch()) {
    case KEY_LEFT:
      move = TM_LEFT;
      break;
    case KEY_RIGHT:
      move = TM_RIGHT;
      break;
    case KEY_UP:
      move = TM_CLOCK;
      break;
    case KEY_DOWN:
      move = TM_DROP;
      break;
    case 'q':
      running = false;
      move = TM_NONE;
      break;
    case 'p':
      wclear(board);
      box(board, 0, 0);
      wmove(board, tg->rows/2, (tg->cols*COLS_PER_CELL-6)/2);
      wprintw(board, "PAUSED");
      wrefresh(board);
      timeout(-1);
      getch();
      timeout(0);
      move = TM_NONE;
      break;
    case 'b':
      boss_mode();
      move = TM_NONE;
      break;
    case 's':
      save(tg, board);
      move = TM_NONE;
      break;
    case ' ':
      move = TM_HOLD;
      break;
    default:
      move = TM_NONE;
    }
  }

  wclear(stdscr);
  endwin();
  printf("Game over!\n");
  printf("You finished with %d points on level %d.\n", tg->points, tg->level);
  tg_delete(tg);
  return 0;
}
