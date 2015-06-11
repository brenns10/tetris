Tetris
======

A tetris game in C using NCURSES.  It's pretty feature complete, except for
stretch goals I may pick up in my free time.


Instructions
------------

To build, all you need to do is run `make`.  Then run `bin/release/main` to play
the game.

The controls are typical of Tetris:
* `left` and `right`: move the tetromino,
* `up`: rotates (clockwise?) the tetromino,
* `down`: immediately drops the tetromino (not a fast drop, an immediate drop),
* `q`: exit the game prematurely,
* `p`: pause the game (any key to resume),
* `b`: "boss mode" - shows a mock terminal screen to fool nosy onlookers.  Hit
  `F1` to resume the game afterwards.


Future/Stretch Goals
--------------------

* Store a block.
* Save/load games.
* MIDI Tetris theme song playback.
* Networked multiplayer!
