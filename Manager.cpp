#include "Manager.h"
#include "TerminalTheme.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

namespace {
//تقرا رقم من اليوزر تتاكد انه بين الrange
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
// بتقرا txt 
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
// بتحول رقم اوو حرف لقيمه رقميه مش hex 
int hexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }

    return -1;
}
}
// ده ال constructor 
Manager::Manager()
    : usersFile("users.txt"),
      managerUsername("Team32"),
      managerPassword("team32"),
      encryptionKey("Team32SpyGameUsersKey"),
      fileHeader("SPYGAME_USERS_V1") {
    initializeStorage();
}
//بتتاكد اني ال user txt موجود وهعرف اقراه
bool Manager::initializeStorage() {
    ifstream file(usersFile);

    if (!file) {
        vector<UserRecord> emptyUsers;
        return saveUsers(emptyUsers);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string rawContent = buffer.str();

    if (rawContent.find_first_not_of(" \t\r\n") == string::npos) {
        vector<UserRecord> emptyUsers;
        return saveUsers(emptyUsers);
    }

    vector<UserRecord> users;

    if (parseEncryptedUsers(rawContent, users)) {
        return true;
    }

    if (parseLegacyUsers(rawContent, users)) {
        return saveUsers(users);
    }

    TerminalTheme::printStatus(
        "WARNING",
        "users.txt could not be read safely.",
        "red"
    );
    return false;
}
// بتبدا تقرااليوزرز من الملف وتفك ال encrypted وتحطهم في فيكتور
bool Manager::loadUsers(vector<UserRecord>& users) const {
    users.clear();

    ifstream file(usersFile);
    if (!file) {
        return false;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string rawContent = buffer.str();

    if (rawContent.find_first_not_of(" \t\r\n") == string::npos) {
        return true;
    }

    return parseEncryptedUsers(rawContent, users);
}
//بتحفظ اليوزرز في الفايل
bool Manager::saveUsers(const vector<UserRecord>& users) const {
    ofstream file(usersFile, ios::trunc); 
    if (!file) {
        return false;
    }

    string serializedUsers = serializeUsers(users);
    string encryptedUsers = xorTransform(serializedUsers);
    file << hexEncode(encryptedUsers);
    return static_cast<bool>(file);
}
//بتاخد اللي جوه الفايل اللي معموله تشفير وتحوله ليوزرز عاديين
bool Manager::parseEncryptedUsers(
    const string& rawContent,
    vector<UserRecord>& users
) const {
    string encryptedBytes;
    if (!hexDecode(rawContent, encryptedBytes)) {
        return false;
    }

    string decryptedContent = xorTransform(encryptedBytes);
    return deserializeUsers(decryptedContent, users, true);
}
// لو كان الفايل مش متشفر
bool Manager::parseLegacyUsers(
    const string& rawContent,
    vector<UserRecord>& users
) const {
    return deserializeUsers(rawContent, users, false);
}
//بتحول نص عادي ل فيكتور
bool Manager::deserializeUsers(
    const string& plainContent,
    vector<UserRecord>& users,
    bool expectHeader
) const {
    users.clear();

    istringstream stream(plainContent);
    string line;

    if (expectHeader) {
        if (!getline(stream, line) || line != fileHeader) {
            return false;
        }
    }

    while (getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        UserRecord user;
        if (!splitUserRecord(line, user)) {
            return false;
        }

        users.push_back(user);
    }

    return true;
}
//هنحول من فيكتور لنص
string Manager::serializeUsers(const vector<UserRecord>& users) const {
    string serializedUsers = fileHeader + "\n";

    for (size_t i = 0; i < users.size(); i++) {
        serializedUsers += users[i].username + "," + users[i].passwordHash + "\n";
    }

    return serializedUsers;
}
// تحول الباسورد ل hash
string Manager::hashPassword(const string& password) const {
    unsigned long hash = 5381;

    for (size_t i = 0; i < password.size(); i++) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(password[i]);
    }

    stringstream ss;
    ss << hex << hash;
    return ss.str();
}
//هنعمل encryption عن طريق ال xor 
string Manager::xorTransform(const string& input) const {
    string output = input;

    for (size_t i = 0; i < input.size(); i++) {
        output[i] = static_cast<char>(
            input[i] ^ encryptionKey[i % encryptionKey.size()]
        );
    }

    return output;
}
//بتحول البيانات لشكل hex 
string Manager::hexEncode(const string& input) const {
    static const char* digits = "0123456789ABCDEF";
    string output;
    output.reserve(input.size() * 2);

    for (size_t i = 0; i < input.size(); i++) {
        unsigned char value = static_cast<unsigned char>(input[i]);
        output.push_back(digits[value >> 4]);
        output.push_back(digits[value & 0x0F]);
    }

    return output;
}
//عكس اللي فوق بتحول من hex ل byte
bool Manager::hexDecode(const string& input, string& output) const {
    output.clear();

    string cleanedInput;
    cleanedInput.reserve(input.size());

    for (size_t i = 0; i < input.size(); i++) {
        unsigned char currentChar = static_cast<unsigned char>(input[i]);
        if (!isspace(currentChar)) {
            cleanedInput.push_back(input[i]);
        }
    }

    if (cleanedInput.size() % 2 != 0) {
        return false;
    }

    output.reserve(cleanedInput.size() / 2);

    for (size_t i = 0; i < cleanedInput.size(); i += 2) {
        int high = hexValue(cleanedInput[i]);
        int low = hexValue(cleanedInput[i + 1]);

        if (high == -1 || low == -1) {
            return false;
        }

        output.push_back(static_cast<char>((high << 4) | low));
    }

    return true;
}
// هتبدا تقسم ال line بتاع ال user for username passwordhash 
bool Manager::splitUserRecord(const string& line, UserRecord& record) const {
    size_t commaPosition = line.find(',');

    if (commaPosition == string::npos) {
        return false;
    }

    record.username = line.substr(0, commaPosition);
    record.passwordHash = line.substr(commaPosition + 1);

    return !record.username.empty() && !record.passwordHash.empty();
}
// بتدور علي اليوزر باسمه وترجع مكانه في فيكتور 
int Manager::findUserIndex(
    const vector<UserRecord>& users,
    const string& username
) const {
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].username == username) {
            return static_cast<int>(i);
        }
    }

    return -1;
}
//بتتاكد اني المدير مدخل يوزر وباسورد صح
bool Manager::authenticateManager(
    const string& username,
    const string& password
) const {
    return username == managerUsername && password == managerPassword;
}
//دي بتتاكد هل اليوزر ده فعلا موجود ولا لا
bool Manager::userExists(const string& username) const {
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        return false;
    }

    return findUserIndex(users, username) != -1;
}
// يتتاكد اني اليوزر والباسورد صح
bool Manager::validateUserCredentials(
    const string& username,
    const string& password
) const {
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        return false;
    }

    int userIndex = findUserIndex(users, username);
    if (userIndex == -1) {
        return false;
    }

    return users[userIndex].passwordHash == hashPassword(password);
}
//هنا هتعمل يوزر جديد
bool Manager::createUser(const string& username, const string& password) {
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        return false;
    }

    if (findUserIndex(users, username) != -1) {
        return false;
    }

    UserRecord newUser;
    newUser.username = username;
    newUser.passwordHash = hashPassword(password);
    users.push_back(newUser);

    return saveUsers(users);
}
// بتعرض ال users
void Manager::displayUsers() const {
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        TerminalTheme::printStatus(
            "ERROR",
            "Could not decrypt or read users.txt.",
            "red"
        );
        return;
    }

    if (users.empty()) {
        TerminalTheme::showPanel(
            "Decrypted Users",
            vector<string>{
                "No users were found in the encrypted storage."
            },
            "gold"
        );
        return;
    }

    vector<string> lines;
    for (size_t i = 0; i < users.size(); i++) {
        lines.push_back(
            to_string(i + 1) + ". Username: " + users[i].username
            + " | Password Hash: " + users[i].passwordHash
        );
    }

    TerminalTheme::showPanel("Decrypted Users", lines, "cyan");
}
// تعرض ال يوزر نيم ليوزر موجود
void Manager::editUsername() {
    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Manager Console",
        "EDIT USERNAME",
        "Rename a stored player profile."
    );
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        TerminalTheme::printStatus(
            "ERROR",
            "Could not decrypt or read users.txt.",
            "red"
        );
        return;
    }

    string currentUsernameValue = readRequiredLine("Enter the current username: ");
    int userIndex = findUserIndex(users, currentUsernameValue);

    if (userIndex == -1) {
        TerminalTheme::printStatus(
            "MISSING",
            "User not found.",
            "red"
        );
        return;
    }

    string newUsername = readRequiredLine("Enter the new username: ");

    if (newUsername.find(',') != string::npos) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Username cannot contain commas.",
            "red"
        );
        return;
    }

    if (findUserIndex(users, newUsername) != -1) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "That username is already in use.",
            "red"
        );
        return;
    }

    users[userIndex].username = newUsername;

    if (saveUsers(users)) {
        TerminalTheme::printStatus(
            "SAVED",
            "Username updated successfully.",
            "green"
        );
    } else {
        TerminalTheme::printStatus(
            "ERROR",
            "Failed to save updated users.",
            "red"
        );
    }
}
// تغيرالباسورد ليوزر موجود 
void Manager::editPassword() {
    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Manager Console",
        "EDIT PASSWORD",
        "Rotate the password for an existing user."
    );
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        TerminalTheme::printStatus(
            "ERROR",
            "Could not decrypt or read users.txt.",
            "red"
        );
        return;
    }

    string username = readRequiredLine("Enter the username to update: ");
    int userIndex = findUserIndex(users, username);

    if (userIndex == -1) {
        TerminalTheme::printStatus(
            "MISSING",
            "User not found.",
            "red"
        );
        return;
    }

    string newPassword = readRequiredLine("Enter the new password: ");
    if (newPassword.length() < 4) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Password must be at least 4 characters.",
            "red"
        );
        return;
    }

    string confirmPassword = readRequiredLine("Confirm the new password: ");
    if (newPassword != confirmPassword) {
        TerminalTheme::printStatus(
            "BLOCKED",
            "Passwords do not match.",
            "red"
        );
        return;
    }

    users[userIndex].passwordHash = hashPassword(newPassword);

    if (saveUsers(users)) {
        TerminalTheme::printStatus(
            "SAVED",
            "Password updated successfully.",
            "green"
        );
    } else {
        TerminalTheme::printStatus(
            "ERROR",
            "Failed to save updated users.",
            "red"
        );
    }
}
 
void Manager::deleteUser() {
    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Manager Console",
        "DELETE USER",
        "Permanently remove a stored player profile."
    );
    vector<UserRecord> users;
    if (!loadUsers(users)) {
        TerminalTheme::printStatus(
            "ERROR",
            "Could not decrypt or read users.txt.",
            "red"
        );
        return;
    }

    string username = readRequiredLine("Enter the username to delete: ");
    int userIndex = findUserIndex(users, username);

    if (userIndex == -1) {
        TerminalTheme::printStatus(
            "MISSING",
            "User not found.",
            "red"
        );
        return;
    }

    string confirmation = readRequiredLine("Type YES to confirm deletion: ");
    if (confirmation != "YES") {
        TerminalTheme::printStatus(
            "CANCELLED",
            "Deletion cancelled.",
            "gold"
        );
        return;
    }

    users.erase(users.begin() + userIndex);

    if (saveUsers(users)) {
        TerminalTheme::printStatus(
            "SAVED",
            "User deleted successfully.",
            "green"
        );
    } else {
        TerminalTheme::printStatus(
            "ERROR",
            "Failed to save updated users.",
            "red"
        );
    }
}

void Manager::showManagerMenu() {
    while (true) {
        TerminalTheme::clearScreen();
        TerminalTheme::showHero(
            "Restricted Area",
            "MANAGER PANEL",
            "Inspect and maintain the encrypted player vault."
        );
        TerminalTheme::showMenu(
            "Control Room",
            vector<string>{
                "1. View decrypted users",
                "2. Edit a username",
                "3. Change a password",
                "4. Delete a user",
                "5. Return to the main access console"
            },
            "Choose an admin action."
        );

        int choice = readIntegerInRange("Choose an option: ", 1, 5);

        if (choice == 1) {
            displayUsers();
            TerminalTheme::waitForEnter("Press Enter to return to the manager panel.");
        } else if (choice == 2) {
            editUsername();
            TerminalTheme::waitForEnter("Press Enter to return to the manager panel.");
        } else if (choice == 3) {
            editPassword();
            TerminalTheme::waitForEnter("Press Enter to return to the manager panel.");
        } else if (choice == 4) {
            deleteUser();
            TerminalTheme::waitForEnter("Press Enter to return to the manager panel.");
        } else {
            return;
        }
    }
}

void Manager::showManagerPortal() {
    TerminalTheme::clearScreen();
    TerminalTheme::showHero(
        "Restricted Area",
        "MANAGER LOGIN",
        "Authenticate to enter the control room."
    );

    for (int attempts = 0; attempts < 3; attempts++) {
        string username = readRequiredLine("Username: ");
        string password = readRequiredLine("Password: ");

        if (authenticateManager(username, password)) {
            TerminalTheme::printStatus(
                "ACCESS",
                "Manager access granted.",
                "green"
            );
            showManagerMenu();
            return;
        }

        TerminalTheme::printStatus(
            "DENIED",
            "Invalid manager credentials.",
            "red"
        );

        if (attempts < 2) {
            TerminalTheme::printStatus(
                "RETRY",
                "Try again (" + to_string(2 - attempts)
                + " attempt(s) left).",
                "gold"
            );
        }
    }

    TerminalTheme::printStatus(
        "LOCKED",
        "Too many failed manager attempts.",
        "red"
    );
    TerminalTheme::waitForEnter("Press Enter to return to the access console.");
}
