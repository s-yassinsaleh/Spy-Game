#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <vector>
using namespace std;

class Manager {
private:
    struct UserRecord {
        string username;
        string passwordHash;
    };

    string usersFile;
    const string managerUsername;
    const string managerPassword;
    const string encryptionKey;
    const string fileHeader;

    bool initializeStorage();
    bool loadUsers(vector<UserRecord>& users) const;
    bool saveUsers(const vector<UserRecord>& users) const;
    bool parseEncryptedUsers(const string& rawContent, vector<UserRecord>& users) const;
    bool parseLegacyUsers(const string& rawContent, vector<UserRecord>& users) const;
    bool deserializeUsers(
        const string& plainContent,
        vector<UserRecord>& users,
        bool expectHeader
    ) const;
    string serializeUsers(const vector<UserRecord>& users) const;
    string hashPassword(const string& password) const;
    string xorTransform(const string& input) const;
    string hexEncode(const string& input) const;
    bool hexDecode(const string& input, string& output) const;
    bool splitUserRecord(const string& line, UserRecord& record) const;
    int findUserIndex(const vector<UserRecord>& users, const string& username) const;
    bool authenticateManager(const string& username, const string& password) const;
    void showManagerMenu();
    void displayUsers() const;
    void editUsername();
    void editPassword();
    void deleteUser();

public:
    Manager();
    bool userExists(const string& username) const;
    bool validateUserCredentials(const string& username, const string& password) const;
    bool createUser(const string& username, const string& password);
    void showManagerPortal();
};

#endif
