#include <iostream>
#include <filesystem>

#include "util.h"

#pragma once

std::string promptUsername();

char* promptPassword();

bool isCorrectPassword(std::string username, char* password);

UserContext login(int argc, char *argv[]);
