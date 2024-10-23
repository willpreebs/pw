#include <vector>
#include <fstream>
#include <filesystem>

namespace fileIO {

    void writeToFile(std::ofstream& file, const std::vector<unsigned char>& data);

    std::vector<unsigned char> readFromFile(std::ifstream& file);

    void updateBinaryFile(const std::filesystem::path& path, const std::vector<unsigned char> data);

    void updateTextFile(const std::filesystem::path& path, const std::string& data);

    void writeToNewTextFile(const std::filesystem::path& path, const std::string& data);

    void writeToNewBinaryFile(const std::filesystem::path& path, const std::vector<unsigned char>& data);

    std::vector<unsigned char> readFromTextFile(const std::filesystem::path& path);

    std::vector<unsigned char> readFromBinaryFile(const std::filesystem::path& path);

}
