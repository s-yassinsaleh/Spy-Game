#ifndef VOTEMANAGER_H
#define VOTEMANAGER_H

#include <vector>
using namespace std;

class VoteManager {
private:
    vector<int> votes;
    vector<int> points;
    int numPlayers;

public:
    VoteManager(int n);
    void castVote(int playerIndex);
    int getMajorityVictim();
    void resetVotes();
    void awardPoints(const vector<int>& spyIndices, const vector<int>& guessedPlayers);
    int getPlayerPoints(int playerIndex);
    void displayVotes();
    void displayPoints();
};

#endif
