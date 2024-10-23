#include <iostream>
#include <sys/stat.h>
#include <cstring>
#include <pwd.h>
#include <unistd.h>
#include <filesystem>

#include "encrypt.h"
#include "init.h"
#include "login.h"
#include "logged-in.h"
#include "util.h"

/**
 * TODO:
 * - flags for passing in username and password in initial call
 * - retry system for incorrect passwords
 * 
 */

using namespace std;

namespace fs = filesystem;

void listUserDirs(const fs::path& path) {
    try {
        // Check if the provided path is a directory
        if (!(fs::exists(path) && fs::is_directory(path))) {
            cout << "The .password-store directory does not exist. Run $ pw init" << std::endl;
            return;
        }
        cout << "List of users:\n"; 
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry.path()) && fs::exists(fs::path(entry) / MASTER_FILE_NAME)) {
                cout << entry.path().filename().string() << std::endl;
            }
        }
    }
    catch (const fs::filesystem_error& err) {
        std::cerr << "Filesystem error: " << err.what() << std::endl;
    }
}

// const char *getHomeDir() {
//     struct passwd *pw = getpwuid(getuid());
//     return pw->pw_dir;
// }

// string getPasswordStoreDir() {
//     struct passwd *pw = getpwuid(getuid());
    
//     string homedir (pw->pw_dir);
//     homedir += "/";
//     homedir += PASSWORD_STORE_DIR;
//     homedir += "/";
//     return homedir;
// }

int listUsers() {

    // string dir {getPasswordStoreString()};
    fs::path p = getPasswordStorePath();
    // list directories that exist inside dir
    listUserDirs(p);
    return 0;

}

int tryLogin(int argc, char *argv[]) {
    try {
        UserContext c {login(argc, argv)};
        handleCommands::prompt(c);
    } 
    catch (const char* message) {
        std::cout << message << std::endl;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    cout << "Welcome to the password manager\n";

    if (argc == 1) {
        return tryLogin(argc, argv);
    }
    char const* arg = argv[1];

    if (strcmp(arg, "init") == 0) {
        
        // initialize password storage
        const fs::path storePath = getPasswordStorePath();
        initPasswordStore(argv, storePath);
        setupUsername(argv, storePath);
        return 0;
    }

    else if(strcmp(arg, "login") == 0) {
        return tryLogin(argc, argv);
    }

    else if (strcmp(arg, "userlist") == 0) {
        return listUsers();
    }
    else if (strcmp(arg, "help") == 0) {

    }
    else if (arg[0] == '-') {
        return tryLogin(argc, argv);
    }
    else {
        cout << "Help message WIP\n";
    }
}



