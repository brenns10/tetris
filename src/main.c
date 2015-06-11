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

int curses_main()
{
  tetris_game *tg = tg_create(22, 10);
  tetris_move move;
  bool running = true;

  // NCURSES initialization:
  initscr();             // initialize curses
  cbreak();              // pass key presses to program, but not signals
  noecho();              // don't echo key presses to screen
  keypad(stdscr, TRUE);  // allow arrow keys
  timeout(0);            // no blocking on getch()
  curs_set(0);           // set the cursor to invisible

  // Game loop
  while (running) {
    move(0,0);
    tg_tick(tg, move);
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
      tg_delete(tg);
      running = false;
    default:
      move = TM_NONE;
    }
  }

  endwin();
  return 0;
}

int simple_main()
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

int main(int argc, char *argv[])
{
  if (argc >= 2 && strcmp(argv[1], "curses") == 0) {
    return curses_main();
  } else {
    return simple_main();
  }
}
