#include <cstring>
#include <filesystem>
#include <vector>
#include <openssl/evp.h>

#pragma once

namespace fs = std::filesystem;

namespace encrypt {

    std::vector<unsigned char> encryptPassword(EVP_CIPHER_CTX* ctx, const std::string& master, const std::string& password);

    bool deriveKey(const std::string& password, unsigned char* salt, unsigned char* key);

    std::string decryptPassword(EVP_CIPHER_CTX* ctx, const std::string& master, const fs::path& pFile);

    const std::string decrypt(EVP_CIPHER_CTX* ctx, const std::string master, unsigned char* salt, unsigned char* iv, unsigned char* key, unsigned char *encrypted, size_t e_len);

    std::vector<unsigned char> hashMasterPassword(const std::string& password);

    bool validatePassword(const std::string& pass, const fs::path& filename);

}



