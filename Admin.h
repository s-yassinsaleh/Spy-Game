#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include "Manager.h"
#include "ResultsManager.h"
using namespace std;

class Admin {
private:
    string currentUsername;
    Manager manager;
    ResultsManager resultsManager;

public:
    Admin();
    bool registerUser();
    bool loginUser();
    bool showAuthMenu();         // Shows Login / Register / Exit menu
    string getCurrentUsername(); // Returns the logged-in username
};

#endif
