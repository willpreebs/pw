#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iostream>
#include <fstream>
#include <cstring> // for std::strlen
#include <vector>
#include <filesystem>

#include "encrypt.h"

#define SALT_SIZE 16
#define IV_SIZE 16
#define KEY_SIZE 32

#define ITERATIONS 10000

namespace fs = std::filesystem;

bool encryptAndWritePassword(fs::path filename, const std::string master, const std::string password) {

    size_t len;

    unsigned char result [1024];
    unsigned char salt[SALT_SIZE];
    unsigned char iv[IV_SIZE];


    if (!encryptPassword(master, password, salt, iv, result, &len)) {
        std::cerr << "Password encryption failed" << std::endl;
        return false;
    }

    std::cout << "pc: " << result << std::endl;

    std::cout << "len: " << len << std::endl;

    std::ofstream file (filename, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing.\n";
        return false;
    }

    
    file.write(reinterpret_cast<const char*>(salt), SALT_SIZE);
    file.write(reinterpret_cast<const char*>(iv), IV_SIZE);
    file.write(reinterpret_cast<const char*>(result), len);

    file.close();

    if (!file.good()) {
        std::cerr << "Error: Failed to write to file " << filename << "\n";
        return false;
    }

    std::cout << "Wrote to: " << filename << std::endl;

    return true;

}

bool encryptPassword(const std::string master, 
    const std::string password, unsigned char* salt, unsigned char* iv, unsigned char* result, size_t* encrypted_len) {

    // need to return this somehow or put it directly into the file somehow.
    // return in vector of bytes?
    
    unsigned char key[KEY_SIZE];

    // const int ENCRYPTED_SIZE = password.length() + KEY_SIZE;

    // unsigned char encryptedPassword[ENCRYPTED_SIZE];

    RAND_bytes(salt, SALT_SIZE);
    RAND_bytes(iv, IV_SIZE);


    if (!deriveKey(master, salt, key)) {
        // error handling?
        return false;
    }

    std::cout << "Key: " << key << std::endl;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        std::cerr << "ERROR creating context." << std::endl;
        return false;
    }

    if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, nullptr)) {
        std::cerr << "Error initializing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int len;
    if (!EVP_EncryptUpdate(ctx, result, &len, (unsigned char*)password.c_str(), password.length())) {
        std::cerr << "Error encrypting password." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (!EVP_EncryptFinal_ex(ctx, result + len, &len)) {
        std::cerr << "Error finalizing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);

    std::cout << "EP: " << result << std::endl; 

    // len has the length of the encypted password.
    *encrypted_len = len;

    return true;
}

bool deriveKey(const std::string password, unsigned char* salt, unsigned char* key) {
    if (!PKCS5_PBKDF2_HMAC(password.c_str(), 
        password.size(), salt, SALT_SIZE, ITERATIONS, EVP_sha256(), KEY_SIZE, key)) {
        std::cerr << "Error deriving key" << std::endl;
        return false;
    }
    return true;
}

std::string decryptPassword(const std::string master, const fs::path pFile) {
    std::ifstream in (pFile, std::ios::in | std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
    }

    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());

    std::cout << "Size: " << buffer.size() << std::endl;

    unsigned char salt [SALT_SIZE];

    unsigned char iv [IV_SIZE];

    int eSize = buffer.size() - (SALT_SIZE + IV_SIZE);

    unsigned char* encrypted = new unsigned char[eSize];

    std::copy(buffer.begin(), buffer.begin()+SALT_SIZE, salt);
    std::copy(buffer.begin()+SALT_SIZE, buffer.begin()+SALT_SIZE+IV_SIZE, iv);
    std::copy(buffer.begin()+SALT_SIZE+IV_SIZE, buffer.end(), encrypted);

    unsigned char key[KEY_SIZE];

    if (!deriveKey(master, salt, key)) {
        std::cerr << "Derive Key failed" << std::endl;
        // TODO: ?
    }

    unsigned char result [1024];
    int result_size;
    if (!decrypt(master, result, &result_size, salt, iv, key, encrypted, (size_t) eSize)) {
        // TODO: ?
    }

    std::string decryptedPass (reinterpret_cast<char*>(result), result_size);

    std::cout << "Decrypted password: " << decryptedPass << std::endl;

    delete[] encrypted;

    return decryptedPass;

}

bool decrypt(const std::string master, unsigned char* result, int* result_size, unsigned char* salt, unsigned char* iv, unsigned char* key, unsigned char *encrypted, size_t e_len) {

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        std::cerr << "ERROR creating context." << std::endl;
        return false;
    }

    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        std::cerr << "Error initializing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (!EVP_DecryptUpdate(ctx, result, result_size, encrypted, e_len)) {
        std::cerr << "Error encrypting password." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (!EVP_DecryptFinal_ex(ctx, result + *result_size, result_size)) {
        std::cerr << "Error finalizing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);

    return true;

}

void hashAndStorePassword(const std::string password, const std::string filename) {
    unsigned char hash_buffer[EVP_MAX_MD_SIZE];
    size_t hash_len = 0;

    hashPassword(hash_buffer, &hash_len, password);
    storeBinHashInFile(hash_buffer, hash_len, filename);
}

// Function to hash the password using SHA-256
// improvement: initialize EVP_digest and then reuse instance for more password hashing
void hashPassword(unsigned char* hash, size_t* hash_len, const std::string& password) {

    int result = EVP_Q_digest(NULL, "SHA512", NULL, password.c_str(), password.size(), hash, hash_len);
    
}

// Function to write the hashed password to a file
void storeHashInFile(const std::string& hash, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << hash;
        outFile.close();
        std::cout << "Hashed password stored in file: " << filename << std::endl;
    } else {
        std::cerr << "Error: Could not open file for writing." << std::endl;
    }
}

// Store the hash in a file as its raw bytes
void storeBinHashInFile(const unsigned char* hash, size_t len, const std::string& filename) {
    
    std::ofstream file (filename, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(hash), len);

    file.close();

    if (!file.good()) {
        std::cerr << "Error: Failed to write to file " << filename << std::endl;
    }

}

bool validateHash(const char* pass, const std::string& filename) {
    unsigned char hash_buffer[EVP_MAX_MD_SIZE];
    size_t hash_len = 0;

    hashPassword(hash_buffer, &hash_len, pass);

    std::ifstream file (filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading." << std::endl;
        return false;
    }

    char file_buffer[EVP_MAX_MD_SIZE];

    if (!file.read(file_buffer, EVP_MAX_MD_SIZE)) {
        std::cerr << "Error: Could not read data in bin file" << std::endl;
    }

    if (memcmp(hash_buffer, file_buffer, EVP_MAX_MD_SIZE) == 0) {
        std::cout << "Password correct, access granted." << std::endl;
        return true;
    }
    else {
        std::cout << "Password incorrect" << std::endl;
        return false;
    }
}



// int main() {
//     std::string masterPassword;
//     std::cout << "Enter master password: ";
//     std::getline(std::cin, masterPassword); // Get the password input from user

//     // Hash the password
//     std::string hashedPassword = hashPassword(masterPassword);
//     std::cout << "Hashed Password: " << hashedPassword << std::endl;

//     // Store the hash in a file
//     std::string filename = "password_hash.txt";
//     storeHashInFile(hashedPassword, filename);

//     return 0;
// }


