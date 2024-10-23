#include <iostream>
#include <unistd.h>
#include <filesystem>

#include "login.h"
#include "encrypt.h"
#include "init.h"
#include "util.h"

using namespace std;

namespace fs = filesystem;

string promptUsername() {
    string u {};
    cout << "Enter your username: ";
    // string user {};
    getline(cin, u);

    return u;
}

char *promptPassword() {
    return getpass("Enter your master password: ");
}

bool isCorrectPassword(string username, char* password) {
    return encrypt::validatePassword(password, getPasswordStorePath() / username / MASTER_FILE_NAME);
}

UserContext login(int argc, char *argv[]) {
// prompt user for master password

    // handle login flags
    // -u --username: specify username on login
    // -p --password: specify password on login

    string username {};
    char* password = nullptr;
    
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];
        if (strcmp(arg, "-u") == 0 || strcmp(arg, "--username") == 0) {
            if (i + 1 >= argc) {
                // cout << "Must specify username after flag " << arg << endl;
                throw "Must specify username after flag";
            }
            // set next argument to the username
            username = string(argv[i+1]);
        }
        else if (strcmp(arg, "-p") == 0 || strcmp(arg, "--password") == 0) {
            if (i + 1 >= argc) {
                // cout << "Must specify password after flag " << arg << endl;
                throw "Must specify password after flag";
            }
            password = argv[i+1];
        }
    }

    if (username.empty()) {
        username = promptUsername();
    }

    if (password == nullptr) {
        password = promptPassword();
    }
    
    if (isCorrectPassword(username, password)) {
        return UserContext {username, password, getUsernameDir(username)};
    } 
    else {
        throw "Incorrect password";
    }
}
