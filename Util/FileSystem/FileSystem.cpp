#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

namespace CHTL {

std::string FileSystem::last_error_ = "";

bool FileSystem::exists(const std::string& path) {
    try {
        return std::filesystem::exists(path);
    } catch (const std::exception& e) {
        setLastError("Failed to check if path exists: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::isFile(const std::string& path) {
    try {
        return std::filesystem::is_regular_file(path);
    } catch (const std::exception& e) {
        setLastError("Failed to check if path is file: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::isDirectory(const std::string& path) {
    try {
        return std::filesystem::is_directory(path);
    } catch (const std::exception& e) {
        setLastError("Failed to check if path is directory: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directory(path);
    } catch (const std::exception& e) {
        setLastError("Failed to create directory: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::createDirectories(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        setLastError("Failed to create directories: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::remove(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::exception& e) {
        setLastError("Failed to remove path: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::removeAll(const std::string& path) {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (const std::exception& e) {
        setLastError("Failed to remove all: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::copy(const std::string& from, const std::string& to) {
    try {
        std::filesystem::copy(from, to);
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to copy file: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::move(const std::string& from, const std::string& to) {
    try {
        std::filesystem::rename(from, to);
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to move file: " + std::string(e.what()));
        return false;
    }
}

std::string FileSystem::readFile(const std::string& path) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            setLastError("Failed to open file for reading: " + path);
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } catch (const std::exception& e) {
        setLastError("Failed to read file: " + std::string(e.what()));
        return "";
    }
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    try {
        // 确保目录存在
        std::string dir = getDirectory(path);
        if (!dir.empty() && !exists(dir)) {
            createDirectories(dir);
        }
        
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            setLastError("Failed to open file for writing: " + path);
            return false;
        }
        
        file << content;
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to write file: " + std::string(e.what()));
        return false;
    }
}

bool FileSystem::appendFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path, std::ios::app | std::ios::binary);
        if (!file.is_open()) {
            setLastError("Failed to open file for appending: " + path);
            return false;
        }
        
        file << content;
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to append file: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> FileSystem::readLines(const std::string& path) {
    std::vector<std::string> lines;
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            setLastError("Failed to open file for reading lines: " + path);
            return lines;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
    } catch (const std::exception& e) {
        setLastError("Failed to read lines: " + std::string(e.what()));
    }
    return lines;
}

bool FileSystem::writeLines(const std::string& path, const std::vector<std::string>& lines) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            setLastError("Failed to open file for writing lines: " + path);
            return false;
        }
        
        for (const auto& line : lines) {
            file << line << "\n";
        }
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to write lines: " + std::string(e.what()));
        return false;
    }
}

std::string FileSystem::getCurrentPath() {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::exception& e) {
        setLastError("Failed to get current path: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (const std::exception& e) {
        setLastError("Failed to get absolute path: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::getRelativePath(const std::string& path, const std::string& base) {
    try {
        return std::filesystem::relative(path, base).string();
    } catch (const std::exception& e) {
        setLastError("Failed to get relative path: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::getDirectory(const std::string& path) {
    try {
        return std::filesystem::path(path).parent_path().string();
    } catch (const std::exception& e) {
        setLastError("Failed to get directory: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::getFilename(const std::string& path) {
    try {
        return std::filesystem::path(path).filename().string();
    } catch (const std::exception& e) {
        setLastError("Failed to get filename: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::getStem(const std::string& path) {
    try {
        return std::filesystem::path(path).stem().string();
    } catch (const std::exception& e) {
        setLastError("Failed to get stem: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::getExtension(const std::string& path) {
    try {
        return std::filesystem::path(path).extension().string();
    } catch (const std::exception& e) {
        setLastError("Failed to get extension: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::join(const std::string& path1, const std::string& path2) {
    try {
        return (std::filesystem::path(path1) / path2).string();
    } catch (const std::exception& e) {
        setLastError("Failed to join paths: " + std::string(e.what()));
        return "";
    }
}

std::string FileSystem::normalize(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::exception& e) {
        setLastError("Failed to normalize path: " + std::string(e.what()));
        return "";
    }
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> entries;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            entries.push_back(entry.path().string());
        }
    } catch (const std::exception& e) {
        setLastError("Failed to list directory: " + std::string(e.what()));
    }
    return entries;
}

std::vector<std::string> FileSystem::listFiles(const std::string& path) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        setLastError("Failed to list files: " + std::string(e.what()));
    }
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& path) {
    std::vector<std::string> directories;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        setLastError("Failed to list directories: " + std::string(e.what()));
    }
    return directories;
}

std::vector<std::string> FileSystem::findFiles(const std::string& path, const std::string& pattern) {
    std::vector<std::string> files;
    try {
        std::regex regex_pattern(pattern);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, regex_pattern)) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        setLastError("Failed to find files: " + std::string(e.what()));
    }
    return files;
}

std::vector<std::string> FileSystem::findDirectories(const std::string& path, const std::string& pattern) {
    std::vector<std::string> directories;
    try {
        std::regex regex_pattern(pattern);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_directory()) {
                std::string dirname = entry.path().filename().string();
                if (std::regex_match(dirname, regex_pattern)) {
                    directories.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        setLastError("Failed to find directories: " + std::string(e.what()));
    }
    return directories;
}

size_t FileSystem::getFileSize(const std::string& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::exception& e) {
        setLastError("Failed to get file size: " + std::string(e.what()));
        return 0;
    }
}

std::string FileSystem::getFileHash(const std::string& path) {
    // 简单的文件哈希实现
    std::string content = readFile(path);
    if (content.empty()) {
        return "";
    }
    
    // 使用简单的哈希算法
    size_t hash = 0;
    for (char c : content) {
        hash = hash * 31 + c;
    }
    
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

time_t FileSystem::getLastModified(const std::string& path) {
    try {
        auto ftime = std::filesystem::last_write_time(path);
        return std::chrono::system_clock::to_time_t(
            std::chrono::file_clock::to_sys(ftime));
    } catch (const std::exception& e) {
        setLastError("Failed to get last modified time: " + std::string(e.what()));
        return 0;
    }
}

time_t FileSystem::getLastAccessed(const std::string& path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) {
        return fileStat.st_atime;
    }
    setLastError("Failed to get last accessed time");
    return 0;
}

time_t FileSystem::getCreated(const std::string& path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) {
        return fileStat.st_ctime;
    }
    setLastError("Failed to get created time");
    return 0;
}

bool FileSystem::setPermissions(const std::string& path, int permissions) {
    try {
        std::filesystem::permissions(path, static_cast<std::filesystem::perms>(permissions));
        return true;
    } catch (const std::exception& e) {
        setLastError("Failed to set permissions: " + std::string(e.what()));
        return false;
    }
}

int FileSystem::getPermissions(const std::string& path) {
    try {
        auto perms = std::filesystem::status(path).permissions();
        return static_cast<int>(perms);
    } catch (const std::exception& e) {
        setLastError("Failed to get permissions: " + std::string(e.what()));
        return 0;
    }
}

bool FileSystem::isReadable(const std::string& path) {
    return access(path.c_str(), R_OK) == 0;
}

bool FileSystem::isWritable(const std::string& path) {
    return access(path.c_str(), W_OK) == 0;
}

bool FileSystem::isExecutable(const std::string& path) {
    return access(path.c_str(), X_OK) == 0;
}

std::string FileSystem::createTempFile(const std::string& prefix) {
    try {
        std::string tempDir = std::filesystem::temp_directory_path().string();
        std::string filename = prefix + std::to_string(std::time(nullptr));
        std::string path = join(tempDir, filename);
        
        std::ofstream file(path);
        if (file.is_open()) {
            file.close();
            return path;
        }
    } catch (const std::exception& e) {
        setLastError("Failed to create temp file: " + std::string(e.what()));
    }
    return "";
}

std::string FileSystem::createTempDirectory(const std::string& prefix) {
    try {
        std::string tempDir = std::filesystem::temp_directory_path().string();
        std::string dirname = prefix + std::to_string(std::time(nullptr));
        std::string path = join(tempDir, dirname);
        
        if (createDirectory(path)) {
            return path;
        }
    } catch (const std::exception& e) {
        setLastError("Failed to create temp directory: " + std::string(e.what()));
    }
    return "";
}

bool FileSystem::cleanupTempFiles() {
    // 简单的临时文件清理实现
    // 实际项目中应该维护临时文件列表
    return true;
}

std::string FileSystem::getLastError() {
    return last_error_;
}

void FileSystem::clearLastError() {
    last_error_.clear();
}

void FileSystem::setLastError(const std::string& error) {
    last_error_ = error;
}

} // namespace CHTL