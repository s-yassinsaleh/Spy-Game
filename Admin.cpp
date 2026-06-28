
#include "Admin.h"
#include "TerminalTheme.h"
#include <iostream>
#include <sstream>
using namespace std;

namespace {
int readIntegerInRange(const string& prompt, int minValue, int maxValue) {
    while (true) {
        cout << TerminalTheme::promptText(prompt);

        string line;
        if (!getline(cin, line)) {
            return minValue;
        }

        stringstream ss(line);
        int value;
        char extra;
            // bet5tary anhy option fel awl 5ales fel main menu w lw da5alt 7aga mesh rakmiya aw akbar mn el akbar option aw asghar mn el asghar option by3ml error w by5lyk ted5al tani
        if (ss >> value && !(ss >> extra) &&
            value >= minValue && value <= maxValue) {
            return value;
        }
            
        TerminalTheme::printStatus(
            "INPUT",
            "Please enter a number between " + to_string(minValue)
            + " and " + to_string(maxValue) + ".",
            "red"
        );
    }
}
// beya5od mn el user input w maynf3sh yeb2a fadi
string readRequiredLine(const string& prompt) {
    while (true) {
        cout << TerminalTheme::promptText(prompt);

        string line;
        if (!getline(cin, line)) {
            return "";
        }

        if (!line.empty()) {
            return line;
        }

        TerminalTheme::printStatus(
            "INPUT",
            "Input cannot be empty.",
            "red"
        );
    }
}
}

// Constructor
Admin::Admin() {
    currentUsername = "";
}

// Register Meny
bool Admin::registerUser() {
    string username, password, confirmPassword;

    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Access Console",
        "CREATE AGENT PROFILE",
        "Register a new identity before entering the room."
    );
    username = readRequiredLine("Enter a username: "); //class manager by5od el username w by3ml check 3aleh w lw feh moshkela beya5od el username tani
    //username maynf3sh yeb2a fadi wala yeb2a feh comma
    if (username.find(',') != string::npos) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Username cannot contain commas.",
            "red"
        );
        return false;
    }
    // lw el user meta5ed lazem ye5tar user tani
    if (manager.userExists(username)) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Username already taken. Please try a different one.",
            "red"
        );
        return false;
    }

    password = readRequiredLine("Enter a password: ");

    if (password.length() < 4) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Password must be at least 4 characters.",
            "red"
        );
        return false;
    }

    confirmPassword = readRequiredLine("Confirm password: ");

    if (password != confirmPassword) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Passwords do not match.",
            "red"
        );
        return false;
    }

    if (!manager.createUser(username, password)) {
        TerminalTheme::printStatus(
            "ERROR",
            "Could not save the new user.",
            "red"
        );
        return false;
    }

    currentUsername = username;
    TerminalTheme::printStatus(
        "READY",
        "Account created successfully. Welcome, " + username + ".",
        "green"
    );
    return true;
}


// Login flow

bool Admin::loginUser() {
    string username, password;

    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Access Console",
        "AGENT LOGIN",
        "Sign in and step into the briefing room."
    );
    username = readRequiredLine("Username: ");
    password = readRequiredLine("Password: ");

    if (manager.validateUserCredentials(username, password)) {
        currentUsername = username;
        TerminalTheme::printStatus(
            "ACCESS",
            "Login successful. Welcome back, " + username + ".",
            "green"
        );
        return true;
    }

    TerminalTheme::printStatus(
        "DENIED",
        "Invalid username or password.",
        "red"
    );
    return false;
}


// Auth menu: Login / Register / Exit
// Returns true if user is authenticated, false to quit

bool Admin::showAuthMenu() {
    while (true) {
        TerminalTheme::clearScreen();
        TerminalTheme::showHero(
            "Midnight Briefing",
            "SPY GAME",
            "A cinematic social deduction experience in your terminal."
        );
        TerminalTheme::showMenu(
            "Access Console",
            vector<string>{
                "1. Login with an existing agent profile",
                "2. Register a brand-new player account",
                "3. Open the manager control room",
                "4. Browse the saved mission archive",
                "5. Exit the game"
            },
            "Choose how you want to enter the game."
        );
        int choice = readIntegerInRange("Choose an option: ", 1, 5);

        if (choice == 1) {
            int attempts = 0;
            while (attempts < 3) {
                if (loginUser()) return true;
                attempts++;
                if (attempts < 3) {
                    TerminalTheme::printStatus(
                        "RETRY",
                        "Try again (" + to_string(3 - attempts)
                        + " attempt(s) left).",
                        "gold"
                    );
                }
            }
            TerminalTheme::printStatus(
                "LOCKED",
                "Too many failed attempts.",
                "red"
            );
            TerminalTheme::waitForEnter("Press Enter to return to the access console.");

        } else if (choice == 2) {
            if (registerUser()) {
                return true;
            }

            TerminalTheme::waitForEnter("Press Enter to return to the access console.");

        } else if (choice == 3) {
            manager.showManagerPortal();

        } else if (choice == 4) {
            resultsManager.showSavedResults();

        } else if (choice == 5) {
            TerminalTheme::clearScreen();
            TerminalTheme::showPanel(
                "Goodbye",
                vector<string>{
                    "The briefing room is closing for now.",
                    "Come back when your group is ready for another round of deception."
                },
                "gold"
            );
            return false;
        }
    }
}


// Getter

string Admin::getCurrentUsername() {
    return currentUsername;
}
