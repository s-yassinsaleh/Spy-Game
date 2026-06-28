#include "VoteManager.h"
#include <iostream>
using namespace std;

VoteManager::VoteManager(int n) {
    numPlayers = n;
    votes.resize(numPlayers, 0);
    points.resize(numPlayers, 0);
}

void VoteManager::castVote(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < numPlayers) {
        votes[playerIndex]++;
    } else {
        cout << "Invalid player index." << endl;
    }
}

int VoteManager::getMajorityVictim() {
    int highestVotes = votes[0];
    int victimIndex = 0;
    for (int i = 1; i < numPlayers; i++) {
        if (votes[i] > highestVotes) {
            highestVotes = votes[i];
            victimIndex = i;
        }
    }
    return victimIndex;
}

void VoteManager::resetVotes() {
    for (int i = 0; i < numPlayers; i++) {
        votes[i] = 0;
    }
}

void VoteManager::awardPoints(const vector<int>& spyIndices, const vector<int>& guessedPlayers) {
    bool someoneGuessedSpy = false;

    for (size_t i = 0; i < guessedPlayers.size(); i++) {
        for (size_t j = 0; j < spyIndices.size(); j++) {
            if (guessedPlayers[i] == spyIndices[j]) {
                someoneGuessedSpy = true;
                break;
            }
        }

        if (someoneGuessedSpy) {
            break;
        }
    }

    if (someoneGuessedSpy) {
        for (size_t i = 0; i < guessedPlayers.size(); i++) {
            for (size_t j = 0; j < spyIndices.size(); j++) {
                if (guessedPlayers[i] == spyIndices[j]) {
                    points[i]++;
                    break;
                }
            }
        }
    } else {
        for (size_t i = 0; i < spyIndices.size(); i++) {
            points[spyIndices[i]]++;
        }
    }
}

int VoteManager::getPlayerPoints(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < numPlayers) {
        return points[playerIndex];
    }
    cout << "Invalid player index." << endl;
    return -1;
}

void VoteManager::displayVotes() {
    cout << "Votes:" << endl;
    for (int i = 0; i < numPlayers; i++) {
        cout << "Player " << i << ": " << votes[i] << " vote(s)" << endl;
    }
}

void VoteManager::displayPoints() {
    cout << "Points:" << endl;
    for (int i = 0; i < numPlayers; i++) {
        cout << "Player " << i << ": " << points[i] << " point(s)" << endl;
    }
}
