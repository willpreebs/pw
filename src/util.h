#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <filesystem>

struct UserContext {
    std::string username {};
    std::string master {};
    std::filesystem::path dir {};
};

// std::string getPasswordStoreString();

std::filesystem::path getPasswordStorePath();

std::filesystem::path getUsernameDir(std::string username);

#endif
