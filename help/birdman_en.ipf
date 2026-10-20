:userdoc.
:docprof toc=123456.
:title.Birdman Help

:h1 res=1.Introduction
:p.Birdman is a shooting game for OS/2 Presentation Manager. Birds fly across the screen from right to left and your objective is to shoot them before they escape through the left edge.
:p.The original game was written in 1998 by Mike, Vienna, as an OS/2 screen saver module. This version is the OS2World port (2026) built with Open Watcom, version 1.2.

:h1 res=2.Playing the Game
:p.Choose :hp2.New Game:ehp2. (Ctrl+N) to start. Move the mouse over a bird and press the shoot button to hit it. A bird that reaches the left edge while it is still alive counts as survived.
:p.The title bar shows the number of birds killed and survived and the kill percentage. :hp2.Pause Game:ehp2. (Ctrl+P) stops the birds, :hp2.Quit Game:ehp2. (Ctrl+Q) ends the current game. Shots are ignored while the game is paused.
:p.Pressing the second button twice quickly (double-click) kills every visible bird at once.

:h1 res=3.Features
:ul compact.
:li.Standard Game, Options and Help menus, with a keyboard shortcut for every game action.
:li.:hp2.Key Mapping:ehp2.&colon. the mouse buttons used to shoot one bird and to shoot all birds can be changed (Options menu).
:li.:hp2.Background Run:ehp2.&colon. pauses the game when the window loses the focus.
:li.:hp2.Frame Controls:ehp2.&colon. hides or shows the title bar and the menu bar (Ctrl+F).
:li.:hp2.Save settings on exit:ehp2.&colon. keeps the language, the options and the mouse buttons for the next session.
:li.Six languages&colon. English, Spanish, Dutch, German, French and Italian, for the menus, the messages and this help (Options - Language).
:eul.

:h1 res=4.Keys and Mouse actions
:p.The mouse buttons can be changed with :hp2.Key Mapping...:ehp2. in the :hp2.Options:ehp2. menu. Click a box, then click it again with the mouse button you want to use. The two actions never share a button&colon. if you give one action the button of the other, the other action takes the button you replaced. :hp2.Default:ehp2. restores the left button to shoot one bird and the right button (double-click) to shoot all birds. The choice is saved in BIRDMAN.CFG.
:p.:hp2.Mouse:ehp2.
:dl tsize=34 break=none.
:dt.Left mouse button
:dd.shoot one bird (default).
:dt.Right mouse button, double-click
:dd.shoot all birds (default).
:edl.
:p.:hp2.Keyboard:ehp2.
:dl tsize=10 break=none.
:dt.F1
:dd.Help.
:dt.F2
:dd.Extended help.
:dt.F9
:dd.Keys help (this page).
:dt.F11
:dd.Help index.
:dt.Ctrl+N
:dd.New game.
:dt.Ctrl+P
:dd.Pause / Resume game.
:dt.Ctrl+Q
:dd.End the current game.
:dt.Ctrl+X
:dd.Exit the application.
:dt.Ctrl+B
:dd.Background Run on / off.
:dt.Ctrl+F
:dd.Toggle frame controls (borderless mode).
:edl.

:h1 res=5.License
:p.This program is free software and is distributed under the BSD 3-Clause License. Original game (C) 1998 Mike, Vienna. OS/2 Open Watcom port (C) 2026 OS2World. The full text is in doc\LICENSE.txt.

:h1 res=6.Warranty
:p.This software is provided as-is, without any warranty as to performance or any other warranties whether expressed or implied. The user assumes the entire risk of using the program.
:euserdoc.
