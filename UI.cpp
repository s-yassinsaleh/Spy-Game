#include "UI.h"
#include "TerminalTheme.h"
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
using namespace std;

namespace {
int readInteger(const string& prompt) {
    while (true) {
        cout << TerminalTheme::promptText(prompt);

        string line;
        if (!getline(cin, line)) {
            return 0;
        }

        stringstream ss(line);
        int value;
        char extra;

        if (ss >> value && !(ss >> extra)) {
            return value;
        }

        TerminalTheme::printStatus(
            "INPUT",
            "Please enter a valid number.",
            "red"
        );
    }
}

int readIntegerInRange(const string& prompt, int minValue, int maxValue) {
    while (true) {
        int value = readInteger(prompt);

        if (value >= minValue && value <= maxValue) {
            return value;
        }

        if (maxValue == numeric_limits<int>::max()) {
            TerminalTheme::printStatus(
                "INPUT",
                "Please enter a number greater than or equal to "
                + to_string(minValue) + ".",
                "red"
            );
        } else {
            TerminalTheme::printStatus(
                "INPUT",
                "Please enter a number between " + to_string(minValue)
                + " and " + to_string(maxValue) + ".",
                "red"
            );
        }
    }
}

string readRequiredLine(const string& prompt) {
    while (true) {
        cout << TerminalTheme::promptText(prompt);

        string line;
        if (!getline(cin, line)) {
            return "";
        }

        if (!line.empty()) {
            return line;
        }

        TerminalTheme::printStatus(
            "INPUT",
            "Input cannot be empty.",
            "red"
        );
    }
}

string toLowerCase(const string& input) {
    string output = input;

    for (size_t i = 0; i < output.size(); i++) {
        output[i] = static_cast<char>(
            tolower(static_cast<unsigned char>(output[i]))
        );
    }

    return output;
}
}

void UI::showWelcomeMessage() {
    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Social Deduction Night",
        "SPY GAME",
        "Bluff. Observe. Accuse. Survive."
    );
    TerminalTheme::showPanel(
        "Welcome",
        vector<string>{
            "This round is built for suspense, secret reveals, and dramatic accusations.",
            "Pass the screen carefully when private information appears."
        },
        "gold"
    );
}

int UI::getNumberOfPlayers() {
    while (true) {
        int numberOfPlayers = readInteger("Enter number of players: ");

        if (numberOfPlayers >= 3) {
            return numberOfPlayers;
        }

        TerminalTheme::printStatus(
            "RULE",
            "Enter 3 or more players to start the round.",
            "red"
        );
    }
}

string UI::getPlayerName(int index) {
    return readRequiredLine("Enter name for player " + to_string(index + 1) + ": ");
}

int UI::getNumberOfSpies(int numPlayers) {
    return readIntegerInRange(
        "Enter number of spies: ",
        1,
        numPlayers - 1
    );
}

void UI::showMessage(const string& message) {
    TerminalTheme::printStatus("INFO", message, "cyan");
}
int UI::getVote(const string& playerName) {
    return readIntegerInRange(
        playerName + ", enter your vote: ",
        1,
        numeric_limits<int>::max()
    ) - 1;
}

int UI::getGuess(const string& playerName, int numPlayers) {
    return readIntegerInRange(
        playerName + ", enter the number of the suspected spy (1-"
        + to_string(numPlayers) + "): ",
        1,
        numPlayers
    ) - 1;
}

void UI::showFinalPoints(const string& playerName, int points) {
    TerminalTheme::printStatus(
        "SCORE",
        playerName + ": " + to_string(points) + " point"
        + (points == 1 ? "" : "s"),
        "green"
    );
}

bool UI::askToPlayAgain() {
    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Round Complete",
        "PLAY AGAIN",
        "Shuffle the roles and spin up another tense round."
    );
    TerminalTheme::showPanel(
        "Next Mission",
        vector<string>{
            "Choose yes for a fresh secret word, new spy assignments, and another dramatic showdown."
        },
        "gold"
    );

    while (true) {
        string answer = toLowerCase(
            readRequiredLine("Do you want to play again? (y/n): ")
        );

        if (answer == "y" || answer == "yes") {
            return true;
        }

        if (answer == "n" || answer == "no") {
            return false;
        }

        TerminalTheme::printStatus(
            "INPUT",
            "Please enter y or n.",
            "red"
        );
    }
}
