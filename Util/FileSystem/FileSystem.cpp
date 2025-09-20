#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace CHTL {

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::string FileSystem::getFilename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::vector<std::string> FileSystem::getFilesInDirectoryRecursive(const std::string& path) {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

} // namespace CHTL
