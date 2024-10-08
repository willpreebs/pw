#include <iostream>
#include <filesystem>

#pragma once

std::string promptUsername();

char* promptPassword();

bool confirmPassword(std::filesystem::path passStorePath, std::string username, char* password);
