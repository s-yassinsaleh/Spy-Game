#include "Game.h"
#include "TerminalTheme.h"
#include <iostream>
#include <cstdlib> // for rand() and srand()
using namespace std;

namespace { // helper functions
// betrg3 el asmaa le string array 
vector<string> buildPlayerNames(vector<Player>& players) {
    vector<string> names;

    for (size_t i = 0; i < players.size(); i++) {
        names.push_back(players[i].getName());
    }

    return names;
}
// bey7ot esm el la3eeba w el points ely ma3ahom w lw mafeesh players yet7to message enno mafeesh players w elly 3ndoh akbar points yet7t fe awel el list
vector<pair<string, int> > buildScoreEntries(
    vector<Player>& players,
    VoteManager* voteManager
) {
    vector<pair<string, int> > scores;

    for (size_t i = 0; i < players.size(); i++) {
        scores.push_back(
            make_pair(
                players[i].getName(),
                voteManager->getPlayerPoints(static_cast<int>(i))
            )
        );
    }

    return scores;
}
}

Game::Game() { // default constructor
    voteManager = nullptr; 
    numPlayers = 0;
    numSpies = 0;
    secretWord = "";
}

Game::~Game() {
    delete voteManager;
}

void Game::setupPlayers() {
    clearScreen(); // beymsa7 el shasha
    TerminalTheme::showHero( // bey3ml box fe awel el shasha feha title w subtitle
        "Game Setup",
        "BUILD THE TABLE",
        "Choose the player count, then register everyone for the round."
    );
    TerminalTheme::showPanel( // bey3ml panel feha ma3molat el setup aw el la3eeba
        "Round Configuration",
        vector<string>{
            "Minimum players: 3",
            "The bigger the table, the more chaotic and cinematic the round becomes."
        },
        "gold"
    );
    numPlayers = ui.getNumberOfPlayers();// bey3ml input w byrg3 el number of players ely da5alha el user
    // beyfady ay 7aga adeema
    players.clear();
    spyIndices.clear();
    // lw feh vote manager adeem byms7o w by3ml wa7ed gded 3ashan yb2a ready lel round ely gayya
    delete voteManager;
    voteManager = nullptr;

    clearScreen();
    TerminalTheme::showHero(
        "Game Setup",
        "REGISTER PLAYERS",
        "Create the cast for this round of deception."
    );

    for (int i = 0; i < numPlayers; i++) {
        string name = ui.getPlayerName(i); // beya5od el esm ely da5alha el user lel player
        Player p(name); // bey3ml player gded bel esm da5alha el user
        players.push_back(p); // beyt7at el player da5el el vector ely feha el players
    }

    voteManager = new VoteManager(numPlayers); // beya3ml vote manager gded bel number of players ely da5alha el user
}

void Game::chooseNumberOfSpies() {
    clearScreen();
    TerminalTheme::showHero(
        "Game Setup",
        "SET THE THREAT LEVEL",
        "Decide how many spies are hiding among the group."
    );
    TerminalTheme::showRoster("Registered Players", buildPlayerNames(players));
    numSpies = ui.getNumberOfSpies(numPlayers); // beya5od el number of spies 
}

bool Game::loadSecretWord() {// beya5od kalima seriya randomly mn el word bank w lw m3rfsh yefta7 el file beydy error
    if (!wordBank.loadWordsFromFile()) {
        TerminalTheme::printStatus(
            "ERROR",
            "No secret word was loaded. Please check words.txt.",
            "red"
        );
        return false;
    }

    secretWord = wordBank.getRandomWord(); // class wordbank btw

    if (secretWord.empty()) {
        TerminalTheme::printStatus(
            "ERROR",
            "No secret word was loaded. Please check words.txt.",
            "red"
        );
        return false;
    }

    return true;
}

void Game::assignSpies() {
    spyIndices.clear(); // beyms7 ay spy adeem

    for (int i = 0; i < numPlayers; i++) {
        players[i].assignRole("Normal"); // class player bey5ly kolo normal
    }
    // beyrg3 random index w by7ot el player ely feh el index da spy w by3ml kda le7ad ma yb2a 3ndna 3 spies w by5ly el player ely feh el index da spy
    while (spyIndices.size() < static_cast<size_t>(numSpies)) { //bey3ml loop le 7ad ma yb2a 3ndna spies 3ala 7asb el user input
        int randomIndex = rand() % numPlayers;
        bool alreadySpy = false;

        for (size_t i = 0; i < spyIndices.size(); i++) {
            if (spyIndices[i] == randomIndex) {
                alreadySpy = true;
                break;
            }
        }

        if (!alreadySpy) {
            spyIndices.push_back(randomIndex);
            players[randomIndex].assignRole("Spy");
        }
    }
}

void Game::clearScreen() {
    TerminalTheme::clearScreen();
}

void Game::showWordsToPlayers() {
    clearScreen();
    TerminalTheme::showHero(
        "Private Briefing",
        "ROLE REVEAL",
        "Pass the screen to one player at a time and keep every reveal secret."
    );
    TerminalTheme::showPanel(
        "Mission Flow",
        vector<string>{
            "Each player will privately view their briefing.",
            "If you are a civilian, protect the word and find the spy.",
            "If you are the spy, blend in and stay hidden."
        },
        "gold"
    );
    TerminalTheme::waitForEnter("Press Enter to begin the private reveals.");

    for (int i = 0; i < numPlayers; i++) {
        clearScreen();
        TerminalTheme::showHero(
            "Secure Handoff",
            "PLAYER TURN",
            "Only the named player should be looking at the screen."
        );
        TerminalTheme::showPanel(
            "Prepare Reveal",
            vector<string>{
                "Player: " + players[i].getName(),
                "Make sure the rest of the room looks away before continuing."
            },
            "pink"
        );
        TerminalTheme::waitForEnter("Press Enter to reveal this player's role.");

        clearScreen();
        if (players[i].getRole() == "Spy") {
            TerminalTheme::showPanel(
                "CLASSIFIED ROLE",
                vector<string>{
                    "Player: " + players[i].getName(),
                    "",
                    "[ SPY ]",
                    "Blend in, improvise convincingly, and avoid drawing suspicion."
                },
                "red"
            );
        } else {
            TerminalTheme::showPanel(
                "CLASSIFIED ROLE",
                vector<string>{
                    "Player: " + players[i].getName(),
                    "",
                    "[ CIVILIAN ]",
                    "Secret word: " + secretWord,
                    "Protect the word and expose the spy before they escape."
                },
                "cyan"
            );
        }

        TerminalTheme::waitForEnter("Press Enter to lock the screen and pass it on.");
    }

    clearScreen();
    TerminalTheme::showPanel(
        "Everyone Is Ready",
        vector<string>{
            "All roles have been revealed.",
            "The room is now live. Watch every clue, every hesitation, and every accusation."
        },
        "green"
    );
    TerminalTheme::waitForEnter("Press Enter to move into the accusation phase.");
}

void Game::handleGuessingAndPoints() {
    vector<int> guessedPlayers;
    int correctGuessCount = 0;

    clearScreen();
    TerminalTheme::showHero(
        "Accusation Phase",
        "LOCK YOUR GUESSES",
        "Each player will privately accuse the person they believe is the spy."
    );
    TerminalTheme::showPanel(
        "How Scoring Works",
        vector<string>{
            "Every player who correctly names a spy earns one point.",
            "If nobody identifies a spy, the spy team earns the point instead."
        },
        "gold"
    );
    TerminalTheme::waitForEnter("Press Enter to start the private accusation round.");

    for (int i = 0; i < numPlayers; i++) {
        clearScreen();
        TerminalTheme::showHero(
            "Private Turn",
            "MAKE YOUR ACCUSATION",
            players[i].getName() + ", trust your instincts and pick the most suspicious player."
        );
        TerminalTheme::showRoster("Suspect Board", buildPlayerNames(players));
        int guess = ui.getGuess(players[i].getName(), numPlayers);
        guessedPlayers.push_back(guess);

        for (size_t j = 0; j < spyIndices.size(); j++) {
            if (guess == spyIndices[j]) {
                correctGuessCount++;
                break;
            }
        }

        clearScreen();
        TerminalTheme::showPanel(
            "Guess Locked",
            vector<string>{
                players[i].getName() + "'s accusation has been recorded.",
                "Pass the screen to the next player."
            },
            "pink"
        );
        TerminalTheme::waitForEnter("Press Enter to continue.");
    }

    voteManager->awardPoints(spyIndices, guessedPlayers); // Class vote manager by7seb el points w by3ml update lel points ely ma3 kol player

    clearScreen();
    if (correctGuessCount > 0) {
        TerminalTheme::showPanel(
            "Round Outcome",
            vector<string>{
                to_string(correctGuessCount)
                + " player(s) correctly identified a spy and earned a point.",
                "The undercover identities are about to be revealed."
            },
            "green"
        );
    } else {
        TerminalTheme::showPanel(
            "Round Outcome",
            vector<string>{
                "Nobody uncovered a spy this round.",
                "The undercover team stayed hidden and claimed the point."
            },
            "red"
        );
    }
    TerminalTheme::waitForEnter("Press Enter for the spy reveal.");
}

void Game::revealSpies() {
    clearScreen();
    vector<string> lines;
    lines.push_back("Secret word: " + secretWord);
    lines.push_back("");

    for (size_t i = 0; i < spyIndices.size(); i++) {
        int spyIndex = spyIndices[i];
        lines.push_back(
            "Player " + to_string(spyIndex + 1) + " - "
            + players[spyIndex].getName() + " was a spy."
        );
    }

    TerminalTheme::showHero(
        "Truth Time",
        "SPY REVEAL",
        "The room finally gets to see who was playing undercover."
    );
    TerminalTheme::showPanel("Undercover Agents", lines, "red");
    TerminalTheme::waitForEnter("Press Enter to open the final leaderboard.");
}

void Game::displayFinalPoints() {
    clearScreen();
    TerminalTheme::showHero(
        "Final Standings",
        "LEADERBOARD",
        "Here is how the round scored after every bluff and accusation."
    );
    TerminalTheme::showLeaderboard(
        "Scoreboard",
        buildScoreEntries(players, voteManager)
    );
}

void Game::saveFinalResults() {
    vector<string> spyNames;
    vector<ResultEntry> resultEntries;

    for (size_t i = 0; i < spyIndices.size(); i++) {
        spyNames.push_back(players[spyIndices[i]].getName());
    }

    for (int i = 0; i < numPlayers; i++) {
        ResultEntry entry;
        entry.playerName = players[i].getName();
        entry.role = players[i].getRole();
        entry.points = voteManager->getPlayerPoints(i);
        resultEntries.push_back(entry);
    }

    if (resultsManager.saveGameResults(secretWord, spyNames, resultEntries)) {
        TerminalTheme::printStatus(
            "ARCHIVE",
            "Final results were saved to results.txt.",
            "green"
        );
    } else {
        TerminalTheme::printStatus(
            "ERROR",
            "Could not save the final results.",
            "red"
        );
    }
}

void Game::playGame() {
    bool playAgain = false;

    ui.showWelcomeMessage();
    TerminalTheme::waitForEnter("Press Enter to start the briefing.");

    do {
        setupPlayers();
        chooseNumberOfSpies();
        if (!loadSecretWord()) {
            return;
        }
        assignSpies();
        showWordsToPlayers();
        handleGuessingAndPoints();
        revealSpies();
        displayFinalPoints();
        saveFinalResults();
        playAgain = ui.askToPlayAgain();
    } while (playAgain);
}
