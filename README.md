# 🕵️ Spy Game

A multiplayer social deduction game developed using **C++** and **Qt** as a team project for the **Object-Oriented Programming course** at Zewail City of Science and Technology.

## About the Game

In Spy Game, most players receive the same secret word, while one or more players are secretly selected as spies.

Players discuss the word without revealing it directly, then vote for the player they believe is the spy. The spies must avoid detection and try to guess the secret word.

## Features

* Player registration and management
* Configurable number of spies
* Random spy selection
* Random secret-word assignment
* Voting system
* Score tracking
* Round and game-flow management
* Admin controls
* Terminal version
* Graphical user interface built with Qt
* Persistent user and result data

## Technologies Used

* C++
* Qt Framework
* Qt Creator
* Object-Oriented Programming
* File Handling
* Git and GitHub

## Object-Oriented Programming Concepts

The project applies several OOP concepts, including:

* Classes and objects
* Encapsulation
* Inheritance
* Polymorphism
* Abstraction
* Separation of responsibilities
* Modular class design

## Main Classes

* `Game` — controls the main game flow and logic
* `Player` — stores player information and score
* `Admin` — provides administrative controls
* `WordBank` — loads and manages secret words
* `VoteManager` — handles player voting
* `ResultsManager` — manages game results
* `Manager` — coordinates game-related operations
* `UI` — handles terminal input and output
* `TerminalTheme` — manages terminal styling

## My Contributions

This project was developed by a team. My main contributions were:

* Developed the `Game` class and implemented the core game flow
* Developed the `Admin` class
* Contributed to the `TerminalTheme` class
* Participated in testing, debugging, and integrating the project components

## Team Members

* Yassin Mohamed Saleh
* Fatma Shaaban
* Kareem Mohamed Elmahdy

## Project Structure

```text
Spy-Game/
├── main.cpp
├── Admin.cpp
├── Admin.h
├── Game.cpp
├── Game.h
├── Manager.cpp
├── Manager.h
├── Player.cpp
├── Player.h
├── ResultsManager.cpp
├── ResultsManager.h
├── TerminalTheme.cpp
├── TerminalTheme.h
├── UI.cpp
├── UI.h
├── VoteManager.cpp
├── VoteManager.h
├── WordBank.cpp
├── WordBank.h
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
├── resources.qrc
├── Spy_game.pro
├── words.txt
├── README.md
└── .gitignore
```

## How to Run the Qt Version

### Requirements

* Qt Creator
* A C++ compiler supported by Qt
* Qt 5 or Qt 6

### Steps

1. Clone the repository:

```bash
git clone https://github.com/s-yassinsaleh/Spy-Game.git
```

2. Open Qt Creator.

3. Open the following project file:

```text
Spy_game.pro
```

4. Configure the available Qt kit.

5. Build and run the project.

## How to Run the Terminal Version

From the repository root, compile the source files using a C++ compiler.

Example:

```bash
g++ -std=c++11 *.cpp -o SpyGame
```

Then run:

```bash
./SpyGame
```

The exact command may differ depending on your operating system and compiler.

## Data Files

* `words.txt` contains the words used during the game.
* User and result files may be generated automatically while the application is running.
* Runtime-generated files are excluded from Git using `.gitignore`.


## Course Information

* Course: Object-Oriented Programming
* Institution: Zewail City of Science and Technology
* Project Type: Team Project

## License

This project is shared for educational and portfolio purposes.
