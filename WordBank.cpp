#include "WordBank.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

WordBank::WordBank() {
    srand(time(0));
}

bool WordBank::loadWordsFromFile() {
    words.clear();

    ifstream file("words.txt");
    if (!file) {
        cout << "Error: Could not open words.txt" << endl;
        return false;
    }

    string word;
    while (file >> word) {
        words.push_back(word);
    }

    file.close();

    if (words.empty()) {
        cout << "No words were loaded from the file." << endl;
        return false;
    }

    return true;
}

string WordBank::getRandomWord() const {
    if (words.empty()) {
        return "";
    }

    int randomIndex = rand() % words.size();
    return words[randomIndex];
}

// Not currently used, kept for future enhancements
void WordBank::displayWords() const {
    if (words.empty()) {
        cout << "No words to display." << endl;
        return;
    }

    cout << "Words in the word bank:" << endl;

    for (size_t i = 0; i < words.size(); i++) {
        cout << words[i] << endl;
    }
}
