#ifndef UI_H
#define UI_H

#include <string>
using namespace std;

class UI {
public:
    void showWelcomeMessage();
    int getNumberOfPlayers();
    string getPlayerName(int index);
    int getNumberOfSpies(int numPlayers);
    void showMessage(const string& message);
    void pauseScreen();
    void showPlayerInfo(const string& name, const string& role, const string& word);
    int getVote(const string& playerName);
    int getGuess(const string& playerName, int numPlayers);
    void showFinalPoints(const string& playerName, int points);
    bool askToPlayAgain();
};

#endif
