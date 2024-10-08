#pragma once

#include <iostream>
#include <filesystem>

#define PASSWORD_FILE_NAME "password.bin"
#define USERNAME_FILE_NAME "username"

struct UserContext {
    std::string username {};
    std::string master {};
    std::filesystem::path dir {};
};

void prompt(std::string username, std::string masterPass);
