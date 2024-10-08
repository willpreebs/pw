
/**
 * After user is logged in, they can run commands:
 * - View their passwords
 *  - view all
 *  - view one
 *  - search for saved passwords
 * - Add more passwords
 *  - add
 *  - set
 * - Edit passwords
 * - Delete passwords
 */

/**
 * TODO:
 * - encrypt and decrypt passwords with master password
 */

#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>
#include <fstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "init.h"
#include "logged-in.h"
#include "util.h"
#include "encrypt.h"

namespace fs = std::filesystem;

bool writeNewUsername(fs::path path, std::string username) {
    std::ofstream uFile (path / USERNAME_FILE_NAME, std::ios::out);

    if (!uFile.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return false;
    }

    size_t len = username.length();
    uFile.write(username.c_str(), len);

    uFile.close();

    if (!uFile.good()) {
        std::cerr << "Error: Failed to write new username to file." << std::endl;
        return false;
    }
    return true;
}

// assume path is to directory containing username file
// TODO: parameterize max username len
// assume username is not longer than 256
bool readUsername(fs::path path, char* buffer, size_t buffer_size, size_t* read) {
    std::ifstream uFile(path / USERNAME_FILE_NAME, std::ios::in);

    // std::cout << path / USERNAME_FILE_NAME << std::endl;

    if (!uFile.is_open()) {
        // cerr << "Error: Could not open file for writing.\n";
        return false;
    }

    std::vector<unsigned char> buffer_vec((std::istreambuf_iterator<char>(uFile)),
                               std::istreambuf_iterator<char>());

    //size_t len = username.length();
    // char buffer [256];

    *read = buffer_vec.size();

    if (buffer_vec.size() > buffer_size) {
        std::cerr << "Username in file too large" << std::endl;
        return false;
    }

    for (int i = 0; i < buffer_vec.size(); i++) {
        buffer[i] = buffer_vec[i];
    }

    uFile.close();

    if (!uFile.good()) {
        std::cerr << "Error: Failed to read username file at " << path << std::endl;
        return false;
    }

    return true;
}

std::string getUrl(fs::path uDir) {
    std::string url {};
    std::cout << "Enter a unique identifier or url for the password: ";
    getline(std::cin, url);
    if (url.empty()) {
        std::string response {};
        std::cout << "unique identifier cannot be empty. Try again? (Y/n) ";
        getline(std::cin, response);
        if (response.empty() || response == "y" || response == "Y") {
            return getUrl(uDir);
        }
        else {
            return "";
        }   
    }
    if (fs::exists(uDir / url)) {
        std::cout << "A password has already been saved under the name: " << url << std::endl;
        std::cout << "Try again? (Y/n) ";
        std::string response {};
        getline(std::cin, response);
        if (response.empty() || response == "y" || response == "Y") {
            return getUrl(uDir);
        }
        else {
            return "";
        }   
    }
    return url;
}

std::string getUsername() {
    std::string u {};
    std::cout << "Enter username: ";
    getline(std::cin, u);
    if (u.empty()) {
        std::string response {};
        std::cout << "Username is empty. Proceed? (Y/n) ";
        getline(std::cin, response);
        if (response.empty() || response == "y" || response == "Y") {
            return "";
        }
        else {
            return getUsername();
        }   
    }
    return u;
}


std::string getPassword() {
    std::string p (getpass("Enter password: "));
    if (p.empty()) {
        std::string response {};
        std::cout << "Password cannot be empty. Try again? (Y/n) ";
        getline(std::cin, response);
        if (response.empty() || response == "y" || response == "Y") {
            return getPassword();
        }
        else {
            return "";
        }   
    }
    return p;
}

/**
 * Prompts user to put in the url 
 */
bool addPassword(UserContext u, std::vector<std::string> tokens) {

    fs::path f = u.dir;

    if (!fs::exists(f)) {
        std::cout << "addPassword: username directory doesn't exist at " << f << std::endl;
        return false;
    }

    std::string url = getUrl(f);
    if (url.empty()) {
        return false;
    }

    std::string user = getUsername();
    if (user.empty()) {
        return false;
    }

    std::string p = getPassword();
    if (p.empty()) {
        return false;
    }

    f /= url;

    if (mkdir(f.c_str(), 0777) == -1) {
        std::cout << "Problem with mkdir. New password directory could not be created" << std::endl;
        return false;
    }

    writeNewUsername(f, user);
    return encryptAndWritePassword(f / PASSWORD_FILE_NAME, u.master, p);
}   

void viewPassword(UserContext u, std::vector<std::string> tokens) {
    // TODO: handle flags

    std::string unique {};
    std::cout << "Enter unique identifier password is stored under: ";
    getline(std::cin, unique);

    fs::path uDir = u.dir;

    uDir /= unique;

    if (!fs::exists(uDir / PASSWORD_FILE_NAME)) {
        std::cout << "Password not found" << std::endl;
        return;
    }

    char buffer [256];

    size_t read;

    if (readUsername(uDir, buffer, 256, &read)) {
        std::cout << "Username: " << std::string(reinterpret_cast<char*>(buffer), read) << std::endl;
    }

    std::string decryptedPass = decryptPassword(u.master, uDir / PASSWORD_FILE_NAME);
    
    std::cout << "Password: " << decryptedPass << std::endl;
}

void prompt(std::string username, std::string masterPass) {

    UserContext u = {username, masterPass, getUsernameDir(username)};

    while (true) {
        // prompt
        // cout << bold_on << "pw % " << bold_off;
        std::cout << "\033[1mpw % \033[0m";
        std::string input{};
        getline(std::cin, input);

        std::vector<std::string> tokens;
        boost::split(tokens, input, boost::is_any_of(" "));

        // for (string tok : tokens) {
            
        //     if (tok == "add") {
        //         addPassword(tokens);
        //     }

        // }

        if (tokens[0] == "add") {
            addPassword(u, tokens);
        }
        else if (tokens[0] == "view") {
            viewPassword(u, tokens);
        }

        else if (tokens[0] == "quit") {
            break;
        }
    }
}
