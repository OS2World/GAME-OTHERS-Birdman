Birdman for OS/2 Presentation Manager
======================================
Version 1.2 - Open Watcom port


OVERVIEW
--------
Birdman is a shooting game for OS/2 PM. Birds fly across the screen from
right to left. Your objective is to shoot them before they escape to the
left edge.

Left-click to shoot a bird. Double right-click to kill all birds on screen
(the mouse buttons can be changed in Options > Key Mapping).
The title bar shows your kill rate. Press Ctrl+N to start a new game.

Original author : Mike, Vienna (1998)
OS2World port   : 2026


HOW TO PLAY
-----------
Press Ctrl+N (or Game > New Game) to start.

Birds fly from right to left. Shoot them by left-clicking on a bird.
If a bird reaches the left edge while still alive, it counts as "survived".

Double right-click kills all visible birds at once (cheat mode).

The title bar shows: killed, survived, and kill percentage.


CONTROLS
--------
Ctrl+N   New Game
Ctrl+P   Pause / Resume
Ctrl+Q   Quit current game
Ctrl+X   Exit
Ctrl+B   Background Run toggle
Ctrl+F   Frame Controls toggle (hides title bar and menu)

Left click        Shoot one bird (default)
Double right click Kill all birds (default)

F1   Help
F2   Extended help
F9   Keys help
F11  Help index


KEY MAPPING
-----------
Options > Key Mapping... lets you choose which mouse button shoots one
bird and which button (double-click) shoots all birds. Click the box, then
click it again with the mouse button you want to use. The two actions never
share a button. Default restores left / right. The choice is saved with the
other settings (BIRDMAN.CFG) when "Save settings on exit" is on.


HELP
----
Online help is available in English, Spanish, Dutch, German, French and
Italian (Help menu or F2). The help file follows the selected language and is
found in the help folder next to the executable (help\birdman_xx.hlp).


FILE LIST
---------
bin\birdman.exe   Main executable
bin\help\*.hlp    Online help, one file per language
doc\Readme.txt    This file
doc\Changelog.txt Version history
doc\LICENSE.txt   BSD 3-Clause License


DISCLAIMER
----------
This software is provided "as is" without warranty of any kind.
See doc\LICENSE.txt for full terms.


AUTHORS
-------
Original game: Mike, Vienna (1998)
OS2World port: OS2World (2026)
