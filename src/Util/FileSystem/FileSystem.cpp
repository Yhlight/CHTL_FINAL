#include "Util/FileSystem/FileSystem.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace CHTL {

FileSystem::FileSystem() {
}

FileSystem::~FileSystem() = default;

bool FileSystem::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

bool FileSystem::directoryExists(const std::string& dirPath) {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

std::string FileSystem::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

bool FileSystem::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

std::vector<std::string> FileSystem::listFiles(const std::string& dirPath) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 处理错误
    }
    
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& dirPath) {
    std::vector<std::string> directories;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 处理错误
    }
    
    return directories;
}

bool FileSystem::createDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::create_directories(dirPath);
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

bool FileSystem::removeFile(const std::string& filePath) {
    try {
        return std::filesystem::remove(filePath);
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

bool FileSystem::removeDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::remove_all(dirPath) > 0;
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

std::string FileSystem::getFileExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.extension().string();
}

std::string FileSystem::getFileName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.filename().string();
}

std::string FileSystem::getDirectoryName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.parent_path().string();
}

std::string FileSystem::getAbsolutePath(const std::string& filePath) {
    try {
        return std::filesystem::absolute(filePath).string();
    } catch (const std::filesystem::filesystem_error& e) {
        return filePath;
    }
}

} // namespace CHTL