#include "FileSystem.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace CHTL {
namespace Util {

bool FileSystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileSystem::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        std::cerr << "创建目录失败: " << e.what() << std::endl;
        return false;
    }
}

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "写入文件失败: " << e.what() << std::endl;
        return false;
    }
}

std::string FileSystem::getExtension(const std::string& path) {
    std::filesystem::path p(path);
    return p.extension().string();
}

std::string FileSystem::getFilename(const std::string& path) {
    std::filesystem::path p(path);
    return p.filename().string();
}

std::string FileSystem::getDirectory(const std::string& path) {
    std::filesystem::path p(path);
    return p.parent_path().string();
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
    } catch (const std::exception& e) {
        std::cerr << "列出目录失败: " << e.what() << std::endl;
    }
    return files;
}

bool FileSystem::removeFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::exception& e) {
        std::cerr << "删除文件失败: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystem::removeDirectory(const std::string& path) {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (const std::exception& e) {
        std::cerr << "删除目录失败: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Util
} // namespace CHTL