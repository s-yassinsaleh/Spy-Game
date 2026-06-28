#include "TerminalTheme.h"
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

namespace {
const int kPanelWidth = 78;
const int kInnerWidth = kPanelWidth - 4;
const char* kReset = "\033[0m";
const char* kBold = "\033[1m";
const char* kDim = "\033[2m";
const char* kWhite = "\033[38;2;240;244;255m";
const char* kCyan = "\033[38;2;97;218;251m";
const char* kGold = "\033[38;2;255;196;61m";
const char* kPink = "\033[38;2;255;96;139m";
const char* kGreen = "\033[38;2;103;255;163m";
const char* kRed = "\033[38;2;255;106;106m";

string accentCode(const string& accent) {
    if (accent == "gold") {
        return kGold;
    }

    if (accent == "pink" || accent == "magenta") {
        return kPink;
    }

    if (accent == "green") {
        return kGreen;
    }

    if (accent == "red") {
        return kRed;
    }

    if (accent == "white") {
        return kWhite;
    }

    if (accent == "dim") {
        return kDim;
    }

    return kCyan;
}
// de bet5ly el kalam ya5od 3ard mo3ayan w ye7ot masafa na7yet el ymeen 3ala 7asb el width ely da5el fel parameter
string padRight(const string& text, int width) {
    if (static_cast<int>(text.size()) >= width) {
        return text.substr(0, width);
    }

    return text + string(width - text.size(), ' ');
}
// bet7ot el klam fel nos w bet5ly el masafa na7yet el yemen ad el na7ya el shemal
string centerText(const string& text, int width) {
    if (static_cast<int>(text.size()) >= width) {
        return text.substr(0, width);
    }

    int padding = width - static_cast<int>(text.size());
    int leftPadding = padding / 2;
    int rightPadding = padding - leftPadding;

    return string(leftPadding, ' ') + text + string(rightPadding, ' ');
}
// lw el kalam taweel bet2smo fe kaza satr msh satr wahed 3ala 7asb 3ard el sandoo2
vector<string> wrapLine(const string& text, int width) {
    if (text.empty()) {
        return vector<string>(1, "");
    }

    vector<string> wrapped;
    istringstream words(text); // library sstream 3ashan ye2ra kelma kelma
    string word;
    string currentLine;

    while (words >> word) {
        while (static_cast<int>(word.size()) > width) {
            if (!currentLine.empty()) {
                wrapped.push_back(currentLine);
                currentLine.clear();
            }

            wrapped.push_back(word.substr(0, width));
            word = word.substr(width);
        }

        if (currentLine.empty()) {
            currentLine = word;
        } else if (
            static_cast<int>(currentLine.size() + 1 + word.size()) <= width
        ) {
            currentLine += " " + word;
        } else {
            wrapped.push_back(currentLine);
            currentLine = word;
        }
    }

    if (!currentLine.empty()) {
        wrapped.push_back(currentLine);
    }

    if (wrapped.empty()) {
        wrapped.push_back(text.substr(0, width));
    }

    return wrapped;
}
//de mathln lw 3ndk panel feha 3 goaml kol gomla raweela bett7wl le akter mn satr
void appendWrappedLines(
    vector<string>& destination,
    const vector<string>& sourceLines
) {
    for (size_t i = 0; i < sourceLines.size(); i++) {
        vector<string> wrapped = wrapLine(sourceLines[i], kInnerWidth);
        destination.insert(destination.end(), wrapped.begin(), wrapped.end());
    }
}
// bet3ml shreet keda zeena mathln 3shan ne7dd el sandoo2 hay5ls fen aw haybda2 mnen
void printBorder(const string& accent) {
    cout << accent << "+"
         << string(kPanelWidth - 2, '=')
         << "+"
         << kReset << endl;
}
// nafs ely fo2 bs de 5at wahed msh 5atten
void printSeparator(const string& accent) {
    cout << accent << "|-"
         << string(kPanelWidth - 4, '-')
         << "-|"
         << kReset << endl;
}
// bettb3 satr gowa el sandoo2
void printRow(
    const string& text, // el kalam ely 3ayez ttb3o
    const string& borderAccent, // el lawn ely 3ayez ttb3 beh el border
    const string& textAccent, // el lawn ely 3ayez ttb3 beh el kalam
    bool boldText // 3ayez el kalam yeb2a bold wala la2
) {
    cout << borderAccent << "| " << kReset
         << textAccent;

    if (boldText) {
        cout << kBold;
    }

    cout << padRight(text, kInnerWidth)
         << kReset
         << borderAccent << " |"
         << kReset << endl;
}
}
//betms7 el shasha w y5ly el cursor fe awel el sandoo2
void TerminalTheme::clearScreen() {
    cout << "\033[2J\033[H" << flush; // 
}
// betrg3 prompt melwna ya3ne cout bs be alwan
string TerminalTheme::promptText(const string& label) {
    return accentCode("gold") + string(">> ") + kReset
         + accentCode("white") + label + kReset;
}
// bettb3 el status message ya3ne el kalam ely 3ayez ttb3o w 3aleh label zay [ERROR] aw [INFO] w el lawn ely 3ayez ttb3 beh el label
void TerminalTheme::printStatus(
    const string& label,
    const string& message,
    const string& accent
) {
    cout << accentCode(accent)
         << kBold
         << "[" << label << "] "
         << kReset
         << accentCode("white")
         << message
         << kReset << endl;
}
// bet3ml hero section keda zeena mathln 3ashan te3ml welcome aw el kalam ely 3ayez ttb3o yeb2a akbar w fe nos el sandoo2
void TerminalTheme::showHero(
    const string& eyebrow,
    const string& title,
    const string& subtitle
) {
    cout << accentCode("gold") << string(kPanelWidth, '=') << kReset << endl;

    if (!eyebrow.empty()) {
        cout << accentCode("dim")
             << centerText(eyebrow, kPanelWidth)
             << kReset << endl;
    }

    cout << accentCode("pink")
         << kBold
         << centerText(title, kPanelWidth)
         << kReset << endl;

    if (!subtitle.empty()) {
        cout << accentCode("cyan")
             << centerText(subtitle, kPanelWidth)
             << kReset << endl;
    }

    cout << accentCode("gold") << string(kPanelWidth, '=') << kReset << endl
         << endl;
}
// bet3ml panel feha title w  lines zy el mathln main menu ta7teeha start game w exit mathln
void TerminalTheme::showPanel(
    const string& title,
    const vector<string>& lines,
    const string& accent
) {
    string accentValue = accentCode(accent);
    vector<string> renderedLines;

    appendWrappedLines(renderedLines, lines);

    if (renderedLines.empty()) {
        renderedLines.push_back("");
    }

    printBorder(accentValue);

    if (!title.empty()) {
        printRow(centerText(title, kInnerWidth), accentValue, accentValue, true);
        printSeparator(accentValue);
    }

    for (size_t i = 0; i < renderedLines.size(); i++) {
        printRow(renderedLines[i], accentValue, accentCode("white"), false);
    }

    printBorder(accentValue);
    cout << endl;
}
// bet3ml menu zy el mathln ely fo2 ely feha title w items ta7t el items
void TerminalTheme::showMenu(
    const string& title,
    const vector<string>& items,
    const string& subtitle
) {
    vector<string> lines;

    if (!subtitle.empty()) {
        lines.push_back(subtitle);
        lines.push_back("");
    }

    for (size_t i = 0; i < items.size(); i++) {
        lines.push_back(items[i]);
    }

    showPanel(title, lines, "cyan");
}
// bet3rd asmaa el player w lw mafeesh players yet7to message enno mafeesh players
void TerminalTheme::showRoster(const string& title, const vector<string>& players) {
    vector<string> lines;

    if (players.empty()) {
        lines.push_back("No players have joined this round yet.");
    } else {
        for (size_t i = 0; i < players.size(); i++) {
            lines.push_back(to_string(i + 1) + ". " + players[i]);
        }
    }

    showPanel(title, lines, "pink");
}
// bet3rd el la3eeba w el points ely ma3ahom w lw mafeesh players yet7to message enno mafeesh players w elly 3ndoh akbar points yet7t fe awel el list
void TerminalTheme::showLeaderboard(
    const string& title,
    const vector<pair<string, int> >& entries
) {
    vector<pair<string, int> > sortedEntries = entries; // lw feh nafs el points byertabto 3ala 7asb el asmaa b tartib a-z w lw points mokhtalfa byertabto 3ala 7asb el points

    stable_sort(
        sortedEntries.begin(),
        sortedEntries.end(),
        [](const pair<string, int>& left, const pair<string, int>& right) {
            if (left.second != right.second) {
                return left.second > right.second;
            }

            return left.first < right.first;
        }
    );

    int maxPoints = 0;
    for (size_t i = 0; i < sortedEntries.size(); i++) {
        maxPoints = max(maxPoints, sortedEntries[i].second);
    }

    vector<string> lines;
    if (sortedEntries.empty()) {
        lines.push_back("No scores have been recorded yet.");
    } else {
        for (size_t i = 0; i < sortedEntries.size(); i++) {
            string pointsLabel = to_string(sortedEntries[i].second) + " point";
            if (sortedEntries[i].second != 1) {
                pointsLabel += "s";
            }

            string line = to_string(i + 1) + ". " + sortedEntries[i].first;
            if (line.size() < 28) {
                line += string(28 - line.size(), ' ');
            }

            line += "  " + pointsLabel;

            if (maxPoints > 0 && sortedEntries[i].second > 0) {
                int barLength = 6 + (sortedEntries[i].second * 18 / maxPoints);
                line += "  " + string(barLength, '*');
            }

            if (i == 0) {
                line += "  [TOP AGENT]";
            }

            lines.push_back(line);
        }
    }

    showPanel(title, lines, "green");
}
// betms7 el shasha w y5ly el cursor fe awel el sandoo2 w y5ly el user yedos enter 3ashan ykammel
void TerminalTheme::waitForEnter(const string& message) {
    cout << promptText(message);
    string ignored;
    getline(cin, ignored);
}
//bett2sm stoor 3ala 7asb el newlines 3ashan ttb3 kol gomla fe satr wahed w de bet5ly el kalam ya5od 3ard mo3ayan w ye7ot masafa na7yet el ymeen 3ala 7asb el width ely da5el fel parameter
vector<string> TerminalTheme::splitLines(const string& text) {
    vector<string> lines;
    istringstream stream(text);
    string line;

    while (getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        lines.push_back(line);
    }

    if (lines.empty()) {
        lines.push_back("");
    }

    return lines;
}
