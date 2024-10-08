#include <iostream>
#include <unistd.h>
#include <filesystem>

#include "login.h"
#include "encrypt.h"
#include "init.h"

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

bool confirmPassword(fs::path passStorePath, string username, char* password) {

    return validateHash(password, fs::path(passStorePath / username / MASTER_FILE_NAME));
}
