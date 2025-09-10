#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace FileSystem {

    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            // In a real application, we might want to log an error here.
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    }

    bool fileExists(const std::string& path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

} // namespace FileSystem
