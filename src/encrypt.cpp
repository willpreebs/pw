#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iostream>
#include <fstream>
#include <cstring> // for std::strlen
#include <vector>
#include <filesystem>

#include "encrypt.h"
#include "fileIO.h"

#define ENCRYPTED_SIZE 1024
#define SALT_SIZE 16
#define IV_SIZE 16
#define KEY_SIZE 32

#define ITERATIONS 10000

namespace fs = std::filesystem;

namespace encrypt {

    std::vector<unsigned char> encryptPassword(EVP_CIPHER_CTX* ctx, const std::string& master, const std::string& password) {

        unsigned char result[ENCRYPTED_SIZE];
        unsigned char key[KEY_SIZE];
        unsigned char salt[SALT_SIZE];
        unsigned char iv[IV_SIZE];

        RAND_bytes(salt, SALT_SIZE);
        RAND_bytes(iv, IV_SIZE);


        if (!deriveKey(master, salt, key)) {
            throw "Error deriving key";
        }

        std::cout << "Key: " << reinterpret_cast<const char*>(key) << std::endl;

        if (!ctx) {
            throw "ERROR context does not exist.";
        }

        if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, nullptr)) {
            throw "Error initializing encryption.";
        }

        int len;
        if (!EVP_EncryptUpdate(ctx, result, &len, (unsigned char*)password.c_str(), password.length())) {
            throw "Error encrypting password.";
        }

        if (!EVP_EncryptFinal_ex(ctx, result+len, &len)) {
            throw "Error finalizing encryption.";
        }

        std::vector<unsigned char> r (salt, salt+SALT_SIZE);
        r.insert(r.end(), iv, iv+IV_SIZE);
        r.insert(r.end(), result, result+len);

        return r;
    }

    bool deriveKey(const std::string& password, unsigned char* salt, unsigned char* key) {
        if (!PKCS5_PBKDF2_HMAC(password.c_str(), 
            password.size(), salt, SALT_SIZE, ITERATIONS, EVP_sha256(), KEY_SIZE, key)) {
            std::cerr << "Error deriving key" << std::endl;
            return false;
        }
        return true;
    }
    
    std::string decryptPassword(EVP_CIPHER_CTX* ctx, const std::string& master, const fs::path& pFile) {

        std::vector<unsigned char> buffer = fileIO::readFromBinaryFile(pFile);

        unsigned char salt [SALT_SIZE];
        unsigned char iv [IV_SIZE];
        int eSize = buffer.size() - (SALT_SIZE + IV_SIZE);

        auto encrypted = std::make_unique<unsigned char []>(eSize);

        std::copy(buffer.begin(), buffer.begin()+SALT_SIZE, salt);
        std::copy(buffer.begin()+SALT_SIZE, buffer.begin()+SALT_SIZE+IV_SIZE, iv);
        std::copy(buffer.begin()+SALT_SIZE+IV_SIZE, buffer.end(), encrypted.get());

        unsigned char key[KEY_SIZE];

        if (!deriveKey(master, salt, key)) {
            throw "Derive Key failed";
        }

        // may throw exception
        std::string decryptedPass = decrypt(ctx, master, salt, iv, key, encrypted.get(), (size_t) eSize);

        return decryptedPass;

    }

    const std::string decrypt(EVP_CIPHER_CTX* ctx, const std::string master, unsigned char* salt, unsigned char* iv, unsigned char* key, unsigned char *encrypted, size_t e_len) {

        unsigned char result [1024];
        int result_size;

        if (!ctx) {
            throw "ERROR context does not exist";
        }

        if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
            throw "Error initializing encryption.";
        }

        if (!EVP_DecryptUpdate(ctx, result, &result_size, encrypted, e_len)) {
            throw "Error encrypting password.";
        }

        if (!EVP_DecryptFinal_ex(ctx, result + result_size, &result_size)) {
            throw "Error finalizing encryption.";
        }

        return std::string(reinterpret_cast<const char*>(result), result_size);
        // return std::vector<unsigned char>(result, result+result_size);

    }

    // Function to hash the password using SHA-256
    // improvement: initialize EVP_digest and then reuse instance for more password hashing
    std::vector<unsigned char> hashMasterPassword(const std::string& password) {

        unsigned char hash_buffer[EVP_MAX_MD_SIZE];
        size_t hash_len = 0;

        int result = EVP_Q_digest(NULL, "SHA512", NULL, password.c_str(), password.size(), hash_buffer, &hash_len);

        return std::vector<unsigned char>(hash_buffer, hash_buffer+hash_len);
        
    }


    /**
     * Compares the password to the stored master password and returns true if they match
     */
    bool validatePassword(const std::string& pass, const fs::path& filename) {

        std::vector<unsigned char> passHash = hashMasterPassword(pass);
        std::vector<unsigned char> storedHash = fileIO::readFromBinaryFile(filename);

        return passHash == storedHash;
    }
}    

