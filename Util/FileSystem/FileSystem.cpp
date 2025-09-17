#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace CHTL {

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        // In a real application, you'd want better error handling
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (file.is_open()) {
        file << content;
    }
    // In a real application, you'd want better error handling
}

bool FileSystem::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string FileSystem::getDirectory(const std::string& path) {
    size_t found = path.find_last_of("/\\");
    if (std::string::npos != found) {
        return path.substr(0, found);
    }
    return "."; // Current directory
}

} // namespace CHTL
