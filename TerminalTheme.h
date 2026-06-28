#ifndef TERMINALTHEME_H
#define TERMINALTHEME_H

#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace TerminalTheme {
void clearScreen();
string promptText(const string& label);
void printStatus(
    const string& label,
    const string& message,
    const string& accent
);
void showHero(
    const string& eyebrow,
    const string& title,
    const string& subtitle
);
void showPanel(
    const string& title,
    const vector<string>& lines,
    const string& accent
);
void showMenu(
    const string& title,
    const vector<string>& items,
    const string& subtitle
);
void showRoster(const string& title, const vector<string>& players);
void showLeaderboard(
    const string& title,
    const vector<pair<string, int> >& entries
);
void waitForEnter(const string& message);
vector<string> splitLines(const string& text);
}

#endif
