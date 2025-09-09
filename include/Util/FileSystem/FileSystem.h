#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 文件系统工具类
 * 提供文件系统操作功能
 */
class FileSystem {
public:
    FileSystem();
    ~FileSystem();
    
    // 文件存在性检查
    bool fileExists(const std::string& filePath);
    bool directoryExists(const std::string& dirPath);
    
    // 文件读写
    std::string readFile(const std::string& filePath);
    bool writeFile(const std::string& filePath, const std::string& content);
    
    // 目录操作
    std::vector<std::string> listFiles(const std::string& dirPath);
    std::vector<std::string> listDirectories(const std::string& dirPath);
    bool createDirectory(const std::string& dirPath);
    bool removeDirectory(const std::string& dirPath);
    
    // 文件操作
    bool removeFile(const std::string& filePath);
    
    // 路径操作
    std::string getFileExtension(const std::string& filePath);
    std::string getFileName(const std::string& filePath);
    std::string getDirectoryName(const std::string& filePath);
    std::string getAbsolutePath(const std::string& filePath);

private:
};

} // namespace CHTL