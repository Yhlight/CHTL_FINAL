#include "FileSystem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace CHTL {

FileSystem::FileSystem() : debugMode(false) {
}

FileSystem::FileSystem(const std::string& basePath) : basePath(basePath), debugMode(false) {
}

bool FileSystem::fileExists(const std::string& filename) const {
    std::string fullPath = resolvePath(filename);
    return std::filesystem::exists(fullPath) && std::filesystem::is_regular_file(fullPath);
}

bool FileSystem::exists(const std::string& path) const {
    std::string fullPath = resolvePath(path);
    return std::filesystem::exists(fullPath);
}

bool FileSystem::createFile(const std::string& filename, const std::string& content) {
    std::string fullPath = resolvePath(filename);
    
    try {
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            errors.push_back("Cannot create file: " + fullPath);
            return false;
        }
        
        file << content;
        file.close();
        
        if (debugMode) {
            std::cout << "Created file: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error creating file " + fullPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::deleteFile(const std::string& filename) {
    std::string fullPath = resolvePath(filename);
    
    try {
        if (!fileExists(fullPath)) {
            errors.push_back("File does not exist: " + fullPath);
            return false;
        }
        
        std::filesystem::remove(fullPath);
        
        if (debugMode) {
            std::cout << "Deleted file: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error deleting file " + fullPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::copyFile(const std::string& source, const std::string& destination) {
    std::string sourcePath = resolvePath(source);
    std::string destPath = resolvePath(destination);
    
    try {
        if (!fileExists(sourcePath)) {
            errors.push_back("Source file does not exist: " + sourcePath);
            return false;
        }
        
        std::filesystem::copy_file(sourcePath, destPath);
        
        if (debugMode) {
            std::cout << "Copied file from " << sourcePath << " to " << destPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error copying file from " + sourcePath + " to " + destPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::moveFile(const std::string& source, const std::string& destination) {
    std::string sourcePath = resolvePath(source);
    std::string destPath = resolvePath(destination);
    
    try {
        if (!fileExists(sourcePath)) {
            errors.push_back("Source file does not exist: " + sourcePath);
            return false;
        }
        
        std::filesystem::rename(sourcePath, destPath);
        
        if (debugMode) {
            std::cout << "Moved file from " << sourcePath << " to " << destPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error moving file from " + sourcePath + " to " + destPath + ": " + e.what());
        return false;
    }
}

std::string FileSystem::readFile(const std::string& filename) const {
    std::string fullPath = resolvePath(filename);
    
    if (!fileExists(fullPath)) {
        if (debugMode) {
            std::cerr << "File does not exist: " << fullPath << std::endl;
        }
        return "";
    }
    
    try {
        std::ifstream file(fullPath);
        if (!file.is_open()) {
            if (debugMode) {
                std::cerr << "Cannot open file: " << fullPath << std::endl;
            }
            return "";
        }
        
        std::ostringstream content;
        content << file.rdbuf();
        file.close();
        
        return content.str();
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error reading file " << fullPath << ": " << e.what() << std::endl;
        }
        return "";
    }
}

bool FileSystem::writeFile(const std::string& filename, const std::string& content) {
    std::string fullPath = resolvePath(filename);
    
    try {
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            errors.push_back("Cannot open file for writing: " + fullPath);
            return false;
        }
        
        file << content;
        file.close();
        
        if (debugMode) {
            std::cout << "Wrote file: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error writing file " + fullPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::appendFile(const std::string& filename, const std::string& content) {
    std::string fullPath = resolvePath(filename);
    
    try {
        std::ofstream file(fullPath, std::ios::app);
        if (!file.is_open()) {
            errors.push_back("Cannot open file for appending: " + fullPath);
            return false;
        }
        
        file << content;
        file.close();
        
        if (debugMode) {
            std::cout << "Appended to file: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error appending to file " + fullPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::directoryExists(const std::string& dirname) const {
    std::string fullPath = resolvePath(dirname);
    return std::filesystem::exists(fullPath) && std::filesystem::is_directory(fullPath);
}

bool FileSystem::createDirectory(const std::string& dirname) {
    std::string fullPath = resolvePath(dirname);
    
    try {
        std::filesystem::create_directories(fullPath);
        
        if (debugMode) {
            std::cout << "Created directory: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error creating directory " + fullPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::deleteDirectory(const std::string& dirname) {
    std::string fullPath = resolvePath(dirname);
    
    try {
        if (!directoryExists(fullPath)) {
            errors.push_back("Directory does not exist: " + fullPath);
            return false;
        }
        
        std::filesystem::remove_all(fullPath);
        
        if (debugMode) {
            std::cout << "Deleted directory: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error deleting directory " + fullPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::copyDirectory(const std::string& source, const std::string& destination) {
    std::string sourcePath = resolvePath(source);
    std::string destPath = resolvePath(destination);
    
    try {
        if (!directoryExists(sourcePath)) {
            errors.push_back("Source directory does not exist: " + sourcePath);
            return false;
        }
        
        std::filesystem::copy(sourcePath, destPath, std::filesystem::copy_options::recursive);
        
        if (debugMode) {
            std::cout << "Copied directory from " << sourcePath << " to " << destPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error copying directory from " + sourcePath + " to " + destPath + ": " + e.what());
        return false;
    }
}

bool FileSystem::moveDirectory(const std::string& source, const std::string& destination) {
    std::string sourcePath = resolvePath(source);
    std::string destPath = resolvePath(destination);
    
    try {
        if (!directoryExists(sourcePath)) {
            errors.push_back("Source directory does not exist: " + sourcePath);
            return false;
        }
        
        std::filesystem::rename(sourcePath, destPath);
        
        if (debugMode) {
            std::cout << "Moved directory from " << sourcePath << " to " << destPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error moving directory from " + sourcePath + " to " + destPath + ": " + e.what());
        return false;
    }
}

std::vector<std::string> FileSystem::listDirectory(const std::string& dirname) const {
    std::vector<std::string> items;
    std::string fullPath = resolvePath(dirname);
    
    if (!directoryExists(fullPath)) {
        if (debugMode) {
            std::cerr << "Directory does not exist: " << fullPath << std::endl;
        }
        return items;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(fullPath)) {
            items.push_back(entry.path().filename().string());
        }
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error listing directory " << fullPath << ": " << e.what() << std::endl;
        }
    }
    
    return items;
}

std::vector<std::string> FileSystem::listFiles(const std::string& dirname) const {
    std::vector<std::string> files;
    std::string fullPath = resolvePath(dirname);
    
    if (!directoryExists(fullPath)) {
        if (debugMode) {
            std::cerr << "Directory does not exist: " << fullPath << std::endl;
        }
        return files;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(fullPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error listing files in " << fullPath << ": " << e.what() << std::endl;
        }
    }
    
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& dirname) const {
    std::vector<std::string> directories;
    std::string fullPath = resolvePath(dirname);
    
    if (!directoryExists(fullPath)) {
        if (debugMode) {
            std::cerr << "Directory does not exist: " << fullPath << std::endl;
        }
        return directories;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(fullPath)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error listing directories in " << fullPath << ": " << e.what() << std::endl;
        }
    }
    
    return directories;
}

std::string FileSystem::resolvePath(const std::string& path) const {
    if (basePath.empty()) {
        return path;
    }
    
    std::filesystem::path base(basePath);
    std::filesystem::path file(path);
    
    if (file.is_absolute()) {
        return path;
    }
    
    return (base / file).string();
}

std::string FileSystem::getFileName(const std::string& path) const {
    std::filesystem::path file(path);
    return file.filename().string();
}

std::string FileSystem::getDirectory(const std::string& path) const {
    std::filesystem::path file(path);
    return file.parent_path().string();
}

std::string FileSystem::getFileExtension(const std::string& path) const {
    std::filesystem::path file(path);
    return file.extension().string();
}

std::string FileSystem::getBaseName(const std::string& path) const {
    std::filesystem::path file(path);
    return file.stem().string();
}

bool FileSystem::isAbsolutePath(const std::string& path) const {
    std::filesystem::path file(path);
    return file.is_absolute();
}

bool FileSystem::isRelativePath(const std::string& path) const {
    return !isAbsolutePath(path);
}

void FileSystem::clearErrors() {
    errors.clear();
}

std::string FileSystem::getCurrentDirectory() const {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error getting current directory: " << e.what() << std::endl;
        }
        return "";
    }
}

bool FileSystem::changeDirectory(const std::string& path) {
    std::string fullPath = resolvePath(path);
    
    try {
        std::filesystem::current_path(fullPath);
        
        if (debugMode) {
            std::cout << "Changed directory to: " << fullPath << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        errors.push_back("Error changing directory to " + fullPath + ": " + e.what());
        return false;
    }
}

std::string FileSystem::getAbsolutePath(const std::string& path) const {
    try {
        std::filesystem::path file(path);
        return std::filesystem::absolute(file).string();
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error getting absolute path for " << path << ": " << e.what() << std::endl;
        }
        return path;
    }
}

bool FileSystem::isReadable(const std::string& path) const {
    std::string fullPath = resolvePath(path);
    
    try {
        std::ifstream file(fullPath);
        return file.good();
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error checking if path is readable " << fullPath << ": " << e.what() << std::endl;
        }
        return false;
    }
}

bool FileSystem::isWritable(const std::string& path) const {
    std::string fullPath = resolvePath(path);
    
    try {
        std::ofstream file(fullPath, std::ios::app);
        return file.good();
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error checking if path is writable " << fullPath << ": " << e.what() << std::endl;
        }
        return false;
    }
}

bool FileSystem::isExecutable(const std::string& path) const {
    std::string fullPath = resolvePath(path);
    
    try {
        std::filesystem::path file(fullPath);
        return (std::filesystem::status(file).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Error checking if path is executable " << fullPath << ": " << e.what() << std::endl;
        }
        return false;
    }
}

std::string FileSystem::toString() const {
    std::ostringstream oss;
    oss << "FileSystem{";
    oss << "basePath=" << basePath << ", ";
    oss << "errors=" << errors.size() << ", ";
    oss << "debugMode=" << (debugMode ? "true" : "false");
    oss << "}";
    return oss.str();
}

std::string FileSystem::toDebugString() const {
    std::ostringstream oss;
    oss << "FileSystem Debug Info:\n";
    oss << "  Base Path: " << basePath << "\n";
    oss << "  Current Directory: " << getCurrentDirectory() << "\n";
    oss << "  Errors: " << errors.size() << "\n";
    for (const auto& error : errors) {
        oss << "    " << error << "\n";
    }
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    return oss.str();
}

std::string FileSystem::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"basePath\": \"" << basePath << "\",\n";
    oss << "  \"currentDirectory\": \"" << getCurrentDirectory() << "\",\n";
    oss << "  \"errors\": [\n";
    bool first = true;
    for (const auto& error : errors) {
        if (!first) oss << ",\n";
        oss << "    \"" << error << "\"";
        first = false;
    }
    oss << "\n  ],\n";
    oss << "  \"debugMode\": " << (debugMode ? "true" : "false") << "\n";
    oss << "}";
    return oss.str();
}

std::string FileSystem::toXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<FileSystem>\n";
    oss << "  <basePath>" << basePath << "</basePath>\n";
    oss << "  <currentDirectory>" << getCurrentDirectory() << "</currentDirectory>\n";
    oss << "  <errors>\n";
    for (const auto& error : errors) {
        oss << "    <error>" << error << "</error>\n";
    }
    oss << "  </errors>\n";
    oss << "  <debugMode>" << (debugMode ? "true" : "false") << "</debugMode>\n";
    oss << "</FileSystem>";
    return oss.str();
}

std::string FileSystem::toYAML() const {
    std::ostringstream oss;
    oss << "FileSystem:\n";
    oss << "  basePath: " << basePath << "\n";
    oss << "  currentDirectory: " << getCurrentDirectory() << "\n";
    oss << "  errors:\n";
    for (const auto& error : errors) {
        oss << "    - " << error << "\n";
    }
    oss << "  debugMode: " << (debugMode ? "true" : "false") << "\n";
    return oss.str();
}

FileSystem FileSystem::clone() const {
    FileSystem cloned;
    cloned.basePath = basePath;
    cloned.errors = errors;
    cloned.debugMode = debugMode;
    return cloned;
}

bool FileSystem::operator==(const FileSystem& other) const {
    return basePath == other.basePath &&
           errors == other.errors &&
           debugMode == other.debugMode;
}

bool FileSystem::operator!=(const FileSystem& other) const {
    return !(*this == other);
}

} // namespace CHTL