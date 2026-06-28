#include "ResultsManager.h"
#include "TerminalTheme.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

ResultsManager::ResultsManager() {
    resultsFile = "results.txt";
}

bool ResultsManager::saveGameResults(
    const string& secretWord,
    const vector<string>& spyNames,
    const vector<ResultEntry>& resultEntries
) const {
    ofstream file(resultsFile, ios::app);
    if (!file) {
        return false;
    }

    file << "========================================\n";
    file << "Secret Word: " << secretWord << "\n";
    file << "Spies: ";

    if (spyNames.empty()) {
        file << "None";
    } else {
        for (size_t i = 0; i < spyNames.size(); i++) {
            if (i > 0) {
                file << ", ";
            }
            file << spyNames[i];
        }
    }

    file << "\n";
    file << "Final Results:\n";

    for (size_t i = 0; i < resultEntries.size(); i++) {
        file << i + 1 << ". "
             << resultEntries[i].playerName
             << " | Role: " << resultEntries[i].role
             << " | Points: " << resultEntries[i].points << "\n";
    }

    file << "========================================\n\n";
    return static_cast<bool>(file);
}

void ResultsManager::showSavedResults() const {
    ifstream file(resultsFile);
    if (!file) {
        TerminalTheme::clearScreen();
        TerminalTheme::showHero(
            "Mission Archive",
            "SAVED RESULTS",
            "Review previous rounds and their final scores."
        );
        TerminalTheme::showPanel(
            "Results Archive",
            vector<string>{
                "No saved results were found yet."
            },
            "gold"
        );
        TerminalTheme::waitForEnter("Press Enter to return to the access console.");
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    if (buffer.str().empty()) {
        TerminalTheme::clearScreen();
        TerminalTheme::showHero(
            "Mission Archive",
            "SAVED RESULTS",
            "Review previous rounds and their final scores."
        );
        TerminalTheme::showPanel(
            "Results Archive",
            vector<string>{
                "No saved results were found yet."
            },
            "gold"
        );
        TerminalTheme::waitForEnter("Press Enter to return to the access console.");
        return;
    }

    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Mission Archive",
        "SAVED RESULTS",
        "Every completed round is preserved here for later bragging rights."
    );
    TerminalTheme::showPanel(
        "Results Archive",
        TerminalTheme::splitLines(buffer.str()),
        "cyan"
    );
    TerminalTheme::waitForEnter("Press Enter to return to the access console.");
}
