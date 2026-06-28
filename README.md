# Spy Game

Spy Game is a team project for the Object-Oriented Programming (OOP) course at Zewail City of Science and Technology. The repository contains the original C++ terminal version of the game in the project root and a Qt Widgets desktop version in [`Spy_game/`](Spy_game/).

## Project Overview

Spy Game is a social deduction game where players receive secret words, except for one or more spies. Players discuss, vote, and try to identify the spy before the spy guesses the secret word.



## Features

- Player and manager access flows
- Random secret-word selection from `words.txt`
- Spy assignment and round setup
- Voting flow and round result tracking
- Winner determination and saved results
- Terminal-based version in the repository root
- Qt Widgets GUI version in [`Spy_game/`](Spy_game/)

## Technologies Used

- C++
- Qt Widgets
- qmake
- Object-Oriented Programming (OOP)

## Installation And Build Instructions

### Terminal Version

Build from the repository root so the executable can find [`words.txt`](words.txt):

```bash
g++ -std=c++17   main.cpp Admin.cpp Game.cpp Manager.cpp Player.cpp   ResultsManager.cpp TerminalTheme.cpp UI.cpp VoteManager.cpp WordBank.cpp   -o SpyGame

./SpyGame
```

When you run the terminal version, `users.txt` and `results.txt` are created automatically in the repository root if needed. These runtime-generated files are intentionally ignored and are not committed.

### Qt GUI Version

Open [`Spy_game/Spy_game.pro`](Spy_game/Spy_game.pro) in Qt Creator and build normally, or build from the command line:

```bash
cd Spy_game
mkdir -p build
cd build
qmake ../Spy_game.pro CONFIG+=release
make
open SpyGameQt.app
```

The Qt version embeds its word list through [`resources.qrc`](Spy_game/resources.qrc) and stores generated user/result data in the standard app-data location at runtime instead of relying on hard-coded absolute paths.

## Project Structure

```text
Spy-Game/
├── README.md
├── .gitignore
├── main.cpp
├── Admin.cpp / Admin.h
├── Game.cpp / Game.h
├── Manager.cpp / Manager.h
├── Player.cpp / Player.h
├── ResultsManager.cpp / ResultsManager.h
├── TerminalTheme.cpp / TerminalTheme.h
├── UI.cpp / UI.h
├── VoteManager.cpp / VoteManager.h
├── WordBank.cpp / WordBank.h
├── words.txt
├── Info.txt
├── Spy_game/
│   ├── README.md
│   ├── Spy_game.pro
│   ├── main.cpp
│   ├── mainwindow.cpp / mainwindow.h
│   ├── mainwindow.ui
│   ├── resources.qrc
│   └── words.txt
├── SpyGame Recorder.mov
├── SpyGame_Presentation.pptx
├── Workload_Distribution.pdf
└── words copy.txt
```

## Team Project Notice

This was a team project. The repository is being published as a cleaned source snapshot for portfolio and educational use.

## My Contributions

- Developed the Game class and core game logic.
- Developed the Admin class.
- Contributed to the TerminalTheme class with the team.

## Team Members

- Yassin Mohamed Hamdy
- Kareem Mohamed ElMahdy
- Fatma Shaaban

## Usage Instructions

- Run the terminal version from the repository root if you want the original console flow.
- Build and run the Qt project in [`Spy_game/`](Spy_game/) if you want the GUI version.
- Keep [`words.txt`](words.txt) in the repository root for the terminal build.

## License / Notice

This repository is shared for educational and portfolio purposes.
