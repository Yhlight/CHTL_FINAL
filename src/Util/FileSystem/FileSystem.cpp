#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {
namespace Util {

std::string FileSystem::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open() || !file.good()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::writeFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return file.good();
}

} // namespace Util
} // namespace CHTL
