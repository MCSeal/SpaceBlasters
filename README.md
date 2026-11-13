# Space Blasters

A Space Invaders style game written in C++ with Qt 6. You shoot waves of
enemy ships, and clearing waves captures planets. Capture all three to win.

I made this for my CST8002 programming language research project at
Algonquin College, mainly to learn C++ (classes, inheritance, memory
management) and Qt.



## Controls

- Left / Right arrows or A / D: move
- Space: shoot
- R: restart after a win or game over



## How it plays

- 3 lives. An enemy bullet, or an enemy touching the ship, costs a life.
- Each enemy is worth 10 points. Tougher enemies (drawn dimmer) take two hits
  and are worth 25.
- Shoot down at least half of a wave to light up the next planet and start a
  faster wave. Three planets wins.
- Finished games are saved to `scores.csv` next to the exe, and the best score
  shows on the end screen.

## Building

You need Qt 6 (Widgets, Svg and SvgWidgets modules) and CMake 3.16 or newer.
I build it with MinGW on Windows using VS Code and the CMake Tools extension.

```
cmake -S . -B build
cmake --build build
```

CMake copies the `assets` folder into the build folder when it configures, so
re-run configure after changing any svg. On Windows you may also need to run
`windeployqt` in the build folder so the exe finds the Qt dlls.

## Layout

- `src/Entity` is the base class for everything on screen
- `src/Player`, `Enemy`, `Bullet`, `Planet` and `Effect` (explosions) derive from it
- `src/GameView` runs the game loop, input, waves, score and lives
- `src/HighScore.h` reads and writes `scores.csv`
- `assets/` has the svg sprites, drawn on a 16x16 pixel grid
