#pragma once

#include <iostream>
#include <filesystem>

#include "util.h"

#define PASSWORD_FILE_NAME "password.bin"
#define USERNAME_FILE_NAME "username"
#define MAX_USERNAME_LENGTH 256

namespace handleCommands {
    void prompt(const UserContext&);
   
    bool matchFlag(const std::string& token, const std::vector<const char*> flags);

    std::string readUsername(const std::filesystem::path& path);
}



