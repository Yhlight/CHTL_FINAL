#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace FileSystem {

std::string readFile(const std::string& path) {
    if (!exists(path)) {
        throw std::runtime_error("File does not exist: " + path);
    }
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& path, const std::string& content) {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + path);
    }
    file << content;
}

bool exists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::vector<std::string> listDirectory(const std::string& path) {
    if (!exists(path) || !isDirectory(path)) {
         throw std::runtime_error("Path is not an existing directory: " + path);
    }

    std::vector<std::string> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        entries.push_back(entry.path().filename().string());
    }
    return entries;
}

bool isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

} // namespace FileSystem
