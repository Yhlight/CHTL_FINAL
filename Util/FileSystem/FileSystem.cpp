#include "FileSystem.h"
#include <fstream>
#include <sstream>

namespace CHTL {

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        // In a real scenario, we'd throw an exception or handle the error more gracefully.
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return true;
}

} // namespace CHTL
