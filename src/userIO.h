#include <string>
#include <filesystem>

namespace userIO {
    bool getNewUrl(std::string& url, const std::filesystem::path& uDir);

    bool getExistingUrl(const std::filesystem::path& u, std::string& url);

    bool getUsername(std::string& u);

    bool getPassword(std::string& p);

    bool retryPrompt(const char* p);   

    bool getUpdatedUsername(std::string& user);

    bool getUpdatedPassword(std::string& pass);
}
