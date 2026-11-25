# Testing

## Automated tests

The game rules (score, lives, waves, capture, bolt, speeds) live in `GameState`, and the
high score file code is in `HighScore.h`. Both are tested with Qt Test and no window is opened.

Run from the project folder:

    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure

In VS Code you can also use the CMake Tools "Run CTest" button.

| Test | What it checks |
|---|---|
| startsWithThreeLives | new game has 3 lives and is still playing |
| loseLifeEndsGameAtZero | last life lost = game over, lives never go below 0 |
| wrenchStopsAtFour | wrench takes lives 3 to 4, a second one does nothing |
| scoreAdds | 10 + 25 = 35 |
| captureNeedsHalfTheWave | 4 of 10 fails, 5 of 10 captures, 7 enemies needs 4 |
| thirdCaptureWins | planets 1 and 2 keep playing, planet 3 is a win |
| waveSizesGrow | 2x5, 2x6, 3x7 |
| firstWaveIsPlain | no gaps or tough enemies in wave 1, both appear after |
| boltAddsTwoSpeed | player speed 5 becomes 7 |
| boltOnlyInFirstAndThirdWave | one bolt per wave, none in wave 2 |
| enemiesSpeedUp | faster as enemies die, and faster each wave |
| enemyFireGetsFaster | shot delay drops each wave, never under 30 |
| resetGoesBackToStart | restart puts everything back |
| HighScore: noFileMeansZero | best() is 0 before any game |
| HighScore: saveThenBest | a saved score comes back |
| HighScore: bestIsTheBiggestNotTheLast | best is the max, not the latest |
| HighScore: oldGamesStayInTheFile | each game adds a line `time,score,planets,outcome` |
| HighScore: badLinesAreSkipped | junk lines do not break best() |

The HighScore tests write to a temporary folder, so your real `scores.csv` is not touched.

On Windows, a plain PowerShell window may not find the MinGW compiler, Qt DLLs or ctest. Add them
to the PATH for that window first (adjust the Qt version if yours is different):

    $env:PATH = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.2\mingw_64\bin;C:\Qt\Tools\CMake_64\bin;" + $env:PATH

The VS Code Testing tab and the Run CTest button do this for you.

## Manual play-test checklist

Things the code tests cannot check (drawing, timing, how it feels). Play with the game
built fresh, tick the result, and copy the Pass/Fail column into the test matrix.

Tester: __________  Date: __________  Build: __________

| ID | Do this | Expect | Result | Notes |
|---|---|---|---|---|
| MT-01 | Start the game | window opens, 3 planets dim at the top, SCORE: 0, LIVES: 3, ship at the bottom | | |
| MT-02 | Hold left, then right | ship moves smoothly and stops at both edges | | |
| MT-03 | Hold space | bullets fire at a steady rate, not one per frame | | |
| MT-04 | Shoot an enemy | enemy disappears, score goes up by 10 | | |
| MT-05 | Shoot a faded (tough) enemy | takes two hits, score goes up by 25 | | |
| MT-06 | Shoot enemies in wave 1 | the rest speed up as fewer are left | | |
| MT-07 | Shoot at least half of a wave, let the rest live | next wave appears, first planet lights up | | |
| MT-08 | Shoot less than half then clear the wave some other way (let them hit you) | same wave comes back, no planet | | |
| MT-09 | Play to wave 2 and 3 | waves get bigger (2x5, 2x6, 3x7) and have gaps | | |
| MT-10 | Get hit by an enemy shot | explosion, lives drops by 1, ship changes to the more damaged drawing | | |
| MT-11 | Wait for a wrench while on 3 lives or fewer | falls from the top, catching it shows a blue reverse explosion and lives goes up | | |
| MT-12 | Have 4 lives and wait | no wrench falls | | |
| MT-13 | Let a wrench fall past the ship | it disappears at the bottom with no effect | | |
| MT-14 | Play wave 1 and catch the speed bolt | ship is clearly faster (5 to 7) | | |
| MT-15 | Play wave 2 | no bolt drops | | |
| MT-16 | Lose the last life | ship hides, 3 to 6 explosions in a row, then GAME OVER | | |
| MT-17 | Capture all 3 planets | YOU WIN shows | | |
| MT-18 | Finish a game, check the end text | shows best score, "new best!" only if it beat the old one | | |
| MT-19 | Open scores.csv next to the exe | one new line for each finished game | | |
| MT-20 | Press R after game over | everything resets: score 0, lives 3, planets dim, speed back to normal | | |
| MT-21 | Let the game run for 2 minutes | no crash, no slowdown, no leftover bullets or effects | | |
| MT-23 | Start the game | title screen shows with the best score and the controls, stars drifting behind, nothing else moves | | |
| MT-24 | On the title screen press P, an arrow, then SPACE | only SPACE does anything, game starts without firing a bullet | | |
| MT-25 | Press P while playing, wait, press P again | PAUSED shows, everything freezes (stars too), resumes where it was | | |
| MT-26 | Press P during the death explosions and on the end screen | nothing happens | | |
| MT-27 | Watch the ship while flying | flame flickers, stars scroll at different speeds, nothing looks cut off | | |
| MT-22 | Fresh clone, build following the README | builds and runs without extra steps | | |
