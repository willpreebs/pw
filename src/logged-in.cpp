
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
#include <openssl/evp.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "init.h"
#include "logged-in.h"
#include "util.h"
#include "encrypt.h"
#include "fileIO.h"
#include "userIO.h"

namespace fs = std::filesystem;

class UserLogin {

    private:

        fs::path m_dir;
        std::string m_username;
        std::string m_password;

        EVP_CIPHER_CTX* ctx;  

    public: 
        void addPassword(const fs::path& url, const std::string& password) const {
            auto vec = encrypt::encryptPassword(ctx, m_password, password);
            fileIO::writeToNewBinaryFile(url / PASSWORD_FILE_NAME, vec);
        }

        void updatePassword(const fs::path& url, const std::string& password) const {
            auto vec = encrypt::encryptPassword(ctx, m_password, password);
            fileIO::updateBinaryFile(url, vec);
        }

        bool addUsername(const fs::path& url, const std::string& username) const {
            if (username.empty()) {
                return true;
            }
            try {
                fileIO::writeToNewTextFile(url / USERNAME_FILE_NAME, username);
                return true;
            } catch (const char* message) {
                std::cout << "Error while writing username: " << message << std::endl;
                return false;
            }
        }

        void updateUsername(const std::string& url, const std::string& username) const {
            fileIO::updateTextFile(m_dir / url / USERNAME_FILE_NAME, username);
        }

        void printAllEntries() const {
            if (!fs::exists(m_dir)) {
                std::cerr << "User directory does not exist" << std::endl;
                return;
            }
            for (const auto& entry : fs::directory_iterator(m_dir)) {
                if (fs::is_directory(entry.path()) && fs::exists(entry.path() / PASSWORD_FILE_NAME)) {
                    std::cout << entry.path().filename().string() << std::endl;
                }
            }
        }

        bool doesPasswordExist(const std::string& url) const {
            return fs::exists(m_dir / url);
        }

        // assume path is to directory containing username file
        // TODO: parameterize max username len
        // assume username is not longer than 256
        std::string readUsername(const fs::path& path) const {

            if (!fs::exists(path / USERNAME_FILE_NAME)) {
                return std::string{""};
            }

            auto vec = fileIO::readFromTextFile(path / USERNAME_FILE_NAME);
            return std::string(vec.begin(), vec.end());
        }

        void printEntry(const std::string& url) const {
            try {
                std::string username {readUsername(m_dir / url)};
                if (!username.empty()) {
                    std::cout << "Username: " << username << std::endl;
                } 
            }
            catch (const char* message) {
                std::cout << message << std::endl;
            }

            try {
                std::string decrypted {encrypt::decryptPassword(ctx, m_password, m_dir / url / PASSWORD_FILE_NAME)};
                std::cout << "Password: " << decrypted << std::endl;
            }
            catch (const char* message) {
                std::cout << "Error in printEntry: " << message << std::endl;
            }
        }

        UserLogin(UserContext context) {
            m_dir = context.dir;
            m_username = context.username;
            m_password = context.master;
            ctx = EVP_CIPHER_CTX_new();
        }

        ~UserLogin() {
            EVP_CIPHER_CTX_free(ctx);
        }

        EVP_CIPHER_CTX* getCipherCtx() const {
            return ctx;
        }

        const fs::path& getUserDirectory() const {
            return m_dir;
        }
        
};

namespace handleCommands {
    /**
     * Prompts user to put in the url 
     */
    bool addEntry(const UserLogin& u, const std::vector<std::string>& tokens) {

        // const fs::path& f (u.getUserDirectory());

        // if (!fs::exists(f)) {
        //     std::cout << "add: username directory doesn't exist at " << f << std::endl;
        //     return false;
        // }


        // pw % add newPassword
        // Enter username: ...
        // Enter password: ...

        // pw % add newPassword -u username -p password

        // pw % add newPassword username password

        // pw % add -U newPassword
        // Enter username: ...
        // Enter password: ...
        
        std::string url = {};
        std::string user = {};
        std::string p = {};

        // 
        if (tokens.size() >= 2) {
            if (tokens[1][0] != '-') {
                url = tokens[1];
            }
        }

        for (int i = 0; i < tokens.size()-1; i++) {
            if (tokens[i][0] == '-') {
                if (matchFlag(tokens[i], {"-U", "--url"})) {
                    url = tokens[i+1];
                }
                if (matchFlag(tokens[i], {"-u", "--username"})) {
                    user = tokens[i+1];
                }
                if (matchFlag(tokens[i], {"-p", "--password"})) {
                    p = tokens[i+1];
                }
            }
        }

        const fs::path f = u.getUserDirectory();

        if (!userIO::getNewUrl(url, f)) {
            return false;
        }

        if (!userIO::getUsername(user)) {
            return false;
        }

        if (!userIO::getPassword(p)) {
            return false;
        }

        if (mkdir((f / url).c_str(), 0777) == -1) {
            std::cout << "Problem with mkdir. New password directory could not be created" << std::endl;
            return false;
        }

        if (!u.addUsername(f / url, user)) {
            return false;
        }

        try { 
            u.addPassword((f / url), p);
            return true;
        }
        catch (const char* message) {
            std::cout << message << std::endl;
            return false;
        }
    } 

    // void printAllUrls(UserContext u) {
    //     fs::path p = u.dir;
    //     if (!fs::exists(p)) {
    //         std::cerr << "User directory does not exist" << std::endl;
    //         return;
    //     }
    //     for (const auto& entry : fs::directory_iterator(p)) {
    //         if (fs::is_directory(entry.path()) && fs::exists(entry.path() / PASSWORD_FILE_NAME)) {
    //             std::cout << entry.path().filename().string() << std::endl;
    //         }
    //     }
    // }

    // assume that p is a path that exists

    void viewEntry(const UserLogin& u, std::vector<std::string>& tokens) {
        // TODO: handle flags

        if (tokens.size() == 2) {
            if (tokens[1] == "-a" || tokens[1] == "--all") {
                u.printAllEntries();
                return;
            }
            if (u.doesPasswordExist(tokens[1])) {
                u.printEntry(tokens[1]);
                return;
            }
            else {
                std::cout << "Password under \"" << tokens[1] << "\" does not exist." << std::endl;
                if (!userIO::retryPrompt("Continue?")) {
                    return;
                }
            }
        }

        std::string unique {};
        for (int i = 1; i < tokens.size(); i++) {
            if (tokens[i] == "-U" || tokens[i] == "--url") {
                if (i+1 >= tokens.size()) {
                    std::cout << "Must specify url after flag." << std::endl;
                    return;
                }
                unique = tokens[i+1];
            }
            // TODO: another flag for searching by username?
        }

        if (!userIO::getExistingUrl(u.getUserDirectory(), unique)) {
            return;
        }

        u.printEntry(unique);
    }

    bool matchFlag(const std::string& token, const std::vector<const char*> flags) {
        for (int i = 0; i < flags.size(); i++) {
            if (token == flags[i]) {
                return true;
            }
        }
        return false;
    }

    void updateEntry(const UserLogin& u, std::vector<std::string>& tokens) {
        // flags:
        // -u set new username
        // -p set new password

        std::string url {};
        std::string user {};
        std::string pass {};

        // assume 2nd token is url
        if (tokens.size() < 2) {
            std::cout << "Must specify url to update" << std::endl;
            return;
        }

        url = std::string(tokens[1]);

        if (!u.doesPasswordExist(url)) {
            std::cout << "Cannot update, url does not exist" << std::endl;
            return;
        }

        for (int i = 2; i < tokens.size(); i++) {
            if (matchFlag(tokens[i], {"-u", "--username"})) {
                if (i + 1 >= tokens.size()) {
                    std::cout << "Must specify username after flag." << std::endl;
                    return;
                }
                user = tokens[i+1];
                i++;
            }
            else if (matchFlag(tokens[i], {"-p", "--password"})) {
                if (i + 1 >= tokens.size()) {
                    std::cout << "Must specify password after flag." << std::endl;
                    return;
                }
                pass = tokens[i+1];
                i++;
            }
        }

        if (userIO::getUpdatedUsername(user)) {
            // need to set new username
            try {
                u.updateUsername(url, user);
            } catch (const char* message) {
                std::cerr << "Error while updating username: " << message << std::endl;
                return;
            }
        }

        if (userIO::getUpdatedPassword(pass)) {
            try {
                u.updatePassword(url, pass);
            } catch (const char* message) {
                std::cerr << "Error while updating password: " << message << std::endl;
                return;
            }
        }

        // if url is empty, then assume that 


    }


    void prompt(const UserContext& u) {

        UserLogin l (u);

        while (true) {
            // prompt
            // cout << bold_on << "pw % " << bold_off;
            std::cout << "\033[1mpw % \033[0m";
            std::string input{};
            getline(std::cin, input);

            std::vector<std::string> tokens;
            boost::split(tokens, input, boost::is_any_of(" "));

            if (tokens[0] == "add") {
                addEntry(l, tokens);
            }
            else if (tokens[0] == "view") {
                viewEntry(l, tokens);
            }
            else if (tokens[0] == "update") {
                updateEntry(l, tokens);
            }
    
            else if (tokens[0] == "quit") {
                break;
            }
        }
    }
}
