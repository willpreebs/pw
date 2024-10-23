#include <iostream>
#include <pwd.h>
#include <unistd.h>
#include <filesystem>

#include "init.h"
#include "util.h"

using namespace std;

namespace fs = filesystem;

fs::path getPasswordStorePath() {
    struct passwd *pw = getpwuid(getuid());

    fs::path p (pw->pw_dir);

    p /= PASSWORD_STORE_DIR;

    // cout << "Path cout: " << p << endl;
    return p;
}

// string getPasswordStoreString() {
//     struct passwd *pw = getpwuid(getuid());
    
//     string homedir (pw->pw_dir);

//     fs::path p (homedir);
//     p = p / PASSWORD_STORE_DIR;

//     cout << p;

//     homedir += "/";
//     homedir += PASSWORD_STORE_DIR;
//     homedir += "/";
//     return homedir;
// }
/**
 * return directory of username
 */
fs::path getUsernameDir(string username) {
    return getPasswordStorePath() / username;
}

