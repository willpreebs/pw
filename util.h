#pragma once

#include <iostream>
#include <filesystem>


// std::string getPasswordStoreString();

std::filesystem::path getPasswordStorePath();

fs::path getUsernameDir(std::string username);
