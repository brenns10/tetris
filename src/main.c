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

void display_board(WINDOW *w, tetris_game *obj)
{
  int i, j;
  wclear(w);
  wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
          ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wmove(w,1,1);
  for (i = 0; i < obj->rows; i++) {
    for (j = 0; j < obj->cols; j++) {
      if (TG_IS_BLOCK(tg_get(obj, i, j))) {
        wmove(w, 1 + i, 1 + j*CELLS_PER_BLOCK);
        TG_BLOCK_CURS(w,tg_get(obj, i, j));
      }
    }
  }
  wrefresh(w);
}

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
    wmove(w, c.row + 1, c.col * CELLS_PER_BLOCK + 1);
    TG_BLOCK_CURS(w, TET_TO_BLCK(block.typ));
  }
  wrefresh(w);
}

void display_score(WINDOW *w, tetris_game *tg)
{
  wclear(w);
  wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
          ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wprintw(w, "Score\n%d\n", tg->points);
  wprintw(w, "Level\n%d\n", tg->level);
  wprintw(w, "Lines\n%d\n", tg->lines_remaining);
  wrefresh(w);
}

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
}

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

void init_colors(void)
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

int main(int argc, char **argv)
{
  tetris_game *tg;
  tetris_move move = TM_NONE;
  bool running = true;
  WINDOW *board, *next, *hold, *score;

  if (argc >= 2) {
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
      perror("tetris");
      exit(EXIT_FAILURE);
    }
    tg = tg_load(f);
    fclose(f);
  } else {
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

  board = newwin(tg->rows + 2, 2 * tg->cols + 2, 0, 0);
  next  = newwin(6, 10, 0, 2 * (tg->cols + 1) + 1);
  hold  = newwin(6, 10, 7, 2 * (tg->cols + 1) + 1);
  score = newwin(6, 10, 14, 2 * (tg->cols + 1 ) + 1);

  // Game loop
  while (running) {
    move(0,0);
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
  printw("Game over!\n");
  printw("You finished with %d points on level %d.\n", tg->points, tg->level);
  printw("Press any key to exit.");
  refresh();
  timeout(-1);
  getch();
  endwin();
  tg_delete(tg);
  return 0;
}
