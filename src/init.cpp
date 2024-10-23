#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <filesystem>

#include "encrypt.h"
#include "init.h"
#include "fileIO.h"

using std::string, std::cout, std::endl, std::cin;

namespace fs = std::filesystem;

bool validateNewPass(const char* pass, const char* confirm) {
    // if (strlen(pass) < 7) {
    //     cout << "Password must be at least 7 characters\n";
    //     return false;
    // }

    if (strcmp(pass, confirm) != 0) {
        cout << "Passwords do not match. Try again\n";
        return false;
    }
    return true;
}

int setupUsername(char *argv[], fs::path passStorePath) {

    cout << "Enter your username: ";
    string username {};
    getline(cin, username);

    if (username.size() == 0) {
        cout << "Username cannot be empty.\n";
        return 1;
    }

    struct stat p;
    fs::path path (passStorePath / username);
    cout << "setupUsername path: " << path << endl;

    if (fs::exists(path)) {
        cout << "Username already exists\n";
        cout << "Enter a different username? Y/n\n";
        string r{};
        getline(cin, r);

        if (r.empty() || r == "Y" || r == "y" || r == "yes") {
            setupUsername(argv, passStorePath); 
        }
        else {
            return 0;
        }
    }

    const char* pass = getpass("Enter your master password: ");
    const char* confirm = getpass("Confirm your master password: ");

    while (!validateNewPass(pass, confirm)) {
        pass = getpass("Enter your master password: ");
        confirm = getpass("Confirm your master password: ");
    }    

    if (mkdir(path.c_str(), 0777) == -1) {
        cout << "Problem with mkdir. Password store creation failed\n";
        return 1;
    }

    path /= MASTER_FILE_NAME;
    // path += PASSWORD_FILE_NAME;

    cout << "setupUsername path2: " << path << endl;

    string sPass (pass);

    fileIO::writeToNewBinaryFile(path, encrypt::hashMasterPassword(sPass));

    return 0;
}

/**
 * Returns the path to the .password-store directory
 */
void initPasswordStore(char *argv[], fs::path passStorePath) {

    struct stat sb;

    // test if ~/.password-store exists
    if (stat(passStorePath.c_str(), &sb) != 0) {
        cout << "Creating ~/.password-store directory\n";
        if (mkdir(passStorePath.c_str(), 0777) == -1) {
            cout << "Problem with mkdir. Password store creation failed\n";
            // error handling?
        }
    }
}
