#include <iostream>
#include <filesystem>
#pragma once

#define MASTER_FILE_NAME "master-hash.bin"
#define PASSWORD_STORE_DIR ".password-store"

// using namespace std;

namespace fs = std::filesystem;

bool validateNewPass(const char* pass, const char* confirm);

void initPasswordStore(char *argv[], fs::path passStorePath);

int setupUsername(char *argv[], fs::path passStorePath);
