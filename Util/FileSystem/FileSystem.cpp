#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string FileSystem::getDirectory(const std::string& path) {
    size_t found = path.find_last_of("/\\");
    if (found != std::string::npos) {
        return path.substr(0, found + 1);
    }
    return ""; // No directory part
}

} // namespace CHTL
