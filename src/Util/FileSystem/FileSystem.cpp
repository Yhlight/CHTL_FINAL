#include "FileSystem.h"
#include <fstream>
#include <iostream>

namespace CHTL {

bool FileSystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    return content;
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

std::vector<std::string> FileSystem::listFiles(const std::string& path) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error listing files: " << e.what() << std::endl;
    }
    
    return files;
}

bool FileSystem::removeFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::exception& e) {
        std::cerr << "Error removing file: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystem::removeDirectory(const std::string& path) {
    try {
        return std::filesystem::remove_all(path);
    } catch (const std::exception& e) {
        std::cerr << "Error removing directory: " << e.what() << std::endl;
        return false;
    }
}

} // namespace CHTL