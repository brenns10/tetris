/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 June 2015

  @brief        Main program for tetris.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#include "tetris.h"
#include "util.h"

int main(int argc, char **argv)
{
  tetris_game *tg = tg_create(22, 10);
  tetris_move move = TM_NONE;
  bool running = true;

  // NCURSES initialization:
  initscr();             // initialize curses
  cbreak();              // pass key presses to program, but not signals
  noecho();              // don't echo key presses to screen
  keypad(stdscr, TRUE);  // allow arrow keys
  timeout(0);            // no blocking on getch()
  curs_set(0);           // set the cursor to invisible
  tg_init_colors();      // setup tetris colors

  // Game loop
  while (running) {
    move(0,0);
    running = tg_tick(tg, move);
    tg_curses(tg);
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
      clear();
      printw("PAUSED");
      timeout(-1);
      getch();
      timeout(0);
      move = TM_NONE;
      break;
    default:
      move = TM_NONE;
    }
  }

  printw("\nGame over!\n");
  printw("You finished with %d points on level %d.\n", tg->points, tg->level);
  printw("Press any key to exit.");
  timeout(-1);
  getch();
  endwin();
  tg_delete(tg);
  return 0;
}
