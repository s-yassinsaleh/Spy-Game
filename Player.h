#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

class Player {
private:
    string name;
    string role;
    bool eliminated;

public:
    Player(string n);
    void setName(string n);
    string getName();
    void assignRole(string r);
    string getRole();
    void eliminate();
    bool isEliminated();
    void displayPlayerInfo(); // Not currently used, kept for future enhancements
};

#endif
