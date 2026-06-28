#ifndef WORDBANK_H
#define WORDBANK_H

#include <vector>
#include <string>
using namespace std;

class WordBank {
private:
    vector<string> words;

public:
    WordBank();
    bool loadWordsFromFile();
    string getRandomWord() const;
    void displayWords() const; // Not currently used, kept for future enhancements
};

#endif
