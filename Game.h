#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "Player.h"
#include "WordBank.h"
#include "ResultsManager.h"
#include "VoteManager.h"
#include "UI.h"
using namespace std;

class Game {
private:
    vector<Player> players;
    WordBank wordBank;
    VoteManager* voteManager;
    int numPlayers;
    int numSpies;
    string secretWord;
    vector<int> spyIndices;
    ResultsManager resultsManager;
    UI ui;

public:
    Game();
    ~Game();
    void setupPlayers();
    void chooseNumberOfSpies();
    bool loadSecretWord();
    void assignSpies();
    void clearScreen();
    void showWordsToPlayers();
    void handleGuessingAndPoints();
    void revealSpies();
    void displayFinalPoints();
    void saveFinalResults();
    void playGame();
};

#endif
