#ifndef RESULTSMANAGER_H
#define RESULTSMANAGER_H

#include <string>
#include <vector>
using namespace std;

struct ResultEntry {
    string playerName;
    string role;
    int points;
};

class ResultsManager {
private:
    string resultsFile;

    string getCurrentTimestamp() const;

public:
    ResultsManager();
    bool saveGameResults(
        const string& secretWord,
        const vector<string>& spyNames,
        const vector<ResultEntry>& resultEntries
    ) const;
    void showSavedResults() const;
};

#endif
