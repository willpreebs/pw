#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>

#include "fileIO.h"

namespace fs = std::filesystem;

namespace fileIO {

    void writeToFile(std::ofstream& file, const std::vector<unsigned char>& data) {
        if (!file.is_open()) {
            throw "FileIO Error: Cannot write to file, file is not open";
        }

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    void writeToFile(std::ofstream& file, const std::string& data) {
        if (!file.is_open()) {
            throw "FileIO Error: Cannot write to file, file is not open";
        }

        file.write(data.c_str(), data.size());
    }

    std::vector<unsigned char> readFromFile(std::ifstream& file) {
        if (!file.is_open()) {
            throw "FileIO Error: Cannot read from file, file is not open";
        }

        return std::vector<unsigned char> ((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
    }

    void updateBinaryFile(const fs::path& path, const std::vector<unsigned char> data) {
        if (!fs::exists(path)) {
            throw "FileIO Error: Cannot update file, does not exist";
        }
        std::ofstream file (path, std::ios::out | std::ios::trunc);

        writeToFile(file, data);
    }

    void updateTextFile(const fs::path& path, const std::string& data) {

        if (!fs::exists(path)) {
            throw "FileIO Error: Cannot update file, does not exist";
        }

        std::ofstream file (path, std::ios::out | std::ios::binary | std::ios::trunc);
        writeToFile(file, data);
    }

    void writeToNewTextFile(const fs::path& path, const std::string& data) {
        std::cout << "Writing to new file: " << path << "\n";
        std::ofstream file (path, std::ios::out);
        writeToFile(file, data);
    }
    
    void writeToNewBinaryFile(const fs::path& path, const std::vector<unsigned char>& data) {
        std::cout << path;
        std::ofstream file (path, std::ios::out | std::ios::binary);
        writeToFile(file, data);
    }

    std::vector<unsigned char> readFromTextFile(const fs::path& path) {
        if (!fs::exists(path)) {
            throw "FileIO Error: Cannot read from text file, does not exist";
        } 

        std::ifstream in (path, std::ios::in);

        return readFromFile(in);
    }

    std::vector<unsigned char> readFromBinaryFile(const fs::path& path) {
        if (!fs::exists(path)) {
            throw "FileIO Error: Cannot read from binary file, does not exist";
        } 

        std::ifstream in (path, std::ios::in | std::ios::binary);

        return readFromFile(in);
    }

}
