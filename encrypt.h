#include <cstring>
#include <filesystem>

#pragma once

namespace fs = std::filesystem;

void hashAndStorePassword(const std::string password, const std::string filename);

void hashPassword(unsigned char* hash, size_t* hash_len, const std::string& password);

void storeBinHashInFile(const unsigned char* hash, size_t len, const std::string& filename);

bool validateHash(const char* pass, const std::string& filename);

bool encryptAndWritePassword(fs::path filename, const std::string master, const std::string password);

bool encryptPassword(const std::string master, 
    const std::string password, unsigned char* salt, unsigned char* iv, unsigned char* result, size_t* encrypted_len);

bool deriveKey(const std::string password, unsigned char* salt, unsigned char* key);

std::string decryptPassword(const std::string master, const fs::path pFile);

bool decrypt(const std::string master, unsigned char* result, int* result_size, unsigned char* salt, unsigned char* iv, unsigned char* key, unsigned char *encrypted, size_t e_len);


