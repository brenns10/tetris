Tetris
======

A tetris game in C using NCURSES.  It's pretty feature complete, except for
stretch goals I may pick up in my free time.


Instructions
------------

If you're on Ubuntu, you may need to `sudo apt-get install libncurses5-dev`.

To build, all you need to do is run `make`.  Then run `bin/release/main` to play
the game.

The controls are typical of Tetris:
* `left` and `right`: Move the tetromino,
* `up`: Rotate (clockwise?) the tetromino,
* `down`: Immediately drop the tetromino (not a fast drop, an immediate drop),
* `q`: Exit the game prematurely,
* `p`: Pause the game (any key to resume),
* `b`: "Boss mode" - show a mock terminal screen to fool nosy onlookers.  Hit
  `F1` to resume the game afterwards.
* `s`: Save game and exit (just assumes filename `tetris.save`).  To resume the
  game, run `bin/release/main tetris.save` (or whatever you may have renamed the
  game save to).


Future/Stretch Goals
--------------------

* Store a block.
* MIDI Tetris theme song playback.
* Networked multiplayer!
