#include <iostream>
#include <filesystem>
#include <algorithm>

#include "logged-in.h"
#include <unistd.h>

namespace fs = std::filesystem;

namespace userIO {

    std::string toLowercase(std::string r) {
        std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c) {return std::tolower(c);});
        return r;
    }

    bool isYesOrEmpty(const std::string& response) {
        return response.empty() || response == "y" || response == "Y" || toLowercase(response) == "yes";
    } 

    bool isNoOrEmpty(const std::string& response) {
        return response.empty() || response == "n" || response == "N" || toLowercase(response) == "no";
    } 
    
    bool getNewUrl(std::string& url, const fs::path& uDir) {
        if (!url.empty()) {
            return true;
        }
        std::cout << "Enter a unique identifier or url for the password: ";
        getline(std::cin, url);
        if (url.empty()) {
            std::string response {};
            std::cout << "unique identifier cannot be empty. Try again? (Y/n) ";
            getline(std::cin, response);
            if (isYesOrEmpty(response)) {
                return getNewUrl(url, uDir);
            }
            else {
                return false;
            }   
        }
        if (fs::exists(uDir / url)) {
            std::cout << "A password has already been saved under the name: " << url << std::endl;
            std::cout << "Try again? (Y/n) ";
            std::string response {};
            getline(std::cin, response);
            if (isYesOrEmpty(response)) {
                return getNewUrl(url, uDir);
            }
            else {
                return false;
            }   
        }
        return true;
    }

        
    bool getUsername(std::string& u) {
        if (!u.empty()) {
            return true;
        }
        std::cout << "Enter username: ";
        getline(std::cin, u);
        if (u.empty()) {
            std::string response {};
            std::cout << "Username is empty. Proceed anyway? (Y/n) ";
            getline(std::cin, response);
            if (response.empty() || response == "y" || response == "Y") {
                return true;
            }
            else {
                return getUsername(u);
            }   
        }
        if (u.length() > MAX_USERNAME_LENGTH) {
            std::string response {};
            std::cout << "Username cannot be longer than " << MAX_USERNAME_LENGTH << " characters. Try again? (Y/n)" << std::endl; 
            getline(std::cin, response);
            if (response.empty() || response == "y" || response == "Y") {
                return getUsername(u);
            }
            else {
                return false;
            }   
        }

        return true;
    }

    
    bool getPassword(std::string& p) {
        if (!p.empty()) {
            return true;
        }
        p = std::string(getpass("Enter password: "));
        if (p.empty()) {
            std::string response {};
            std::cout << "Password cannot be empty. Try again? (Y/n) ";
            getline(std::cin, response);
            if (isYesOrEmpty(response)) {
                return getPassword(p);
            }
            else {
                return false;
            }   
        }
        return true;
    }
    
    bool retryPrompt(const char* p) {
        std::string r {};
        std::cout << p << " (Y/n)";
        getline(std::cin, r);
        return isYesOrEmpty(r);
    }

    bool getExistingUrl(const fs::path& uDir, std::string& url) {
        if (url.empty()) {
            std::cout << "Enter unique identifier password is stored under: ";
            getline(std::cin, url);
            if (url.empty()) {
                std::cout << "Url cannot be empty" << std::endl;
                if (!retryPrompt("Try Again?")) {
                    return false;
                }
                else {
                    return getExistingUrl(uDir, url);
                }
            }
        }  
        
        if (!fs::exists(uDir / url)) {
            std::cout << "Url is invalid" << std::endl;
            if (!retryPrompt("Try Again?")) {
                return false;
            }
            else {
                url = "";
                return getExistingUrl(uDir, url);
            }
        } else {
            return true;
        }
    }

    bool getUpdatedUsername(std::string& user) {
        if (user.empty()) {
            std::cout << "Enter the new username to store: ";
            getline(std::cin, user);
            return !user.empty();
        }
        else {
            return true;
        }
    }

    bool getUpdatedPassword(std::string& pass) {
        if (pass.empty()) {
            pass = getpass("Enter the new password to store: ");
            return !pass.empty();
        }
        else {
            return true;
        }
    }
}
