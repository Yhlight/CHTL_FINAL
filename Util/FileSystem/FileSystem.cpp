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

} // namespace CHTL
