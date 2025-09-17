#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace CHTL {

/**
 * 文件系统工具类
 * 提供跨平台的文件操作功能
 */
class FileSystem {
public:
    // 文件操作
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool createDirectories(const std::string& path);
    static bool remove(const std::string& path);
    static bool removeAll(const std::string& path);
    static bool copy(const std::string& from, const std::string& to);
    static bool move(const std::string& from, const std::string& to);
    
    // 文件读写
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool appendFile(const std::string& path, const std::string& content);
    static std::vector<std::string> readLines(const std::string& path);
    static bool writeLines(const std::string& path, const std::vector<std::string>& lines);
    
    // 路径操作
    static std::string getCurrentPath();
    static std::string getAbsolutePath(const std::string& path);
    static std::string getRelativePath(const std::string& path, const std::string& base);
    static std::string getDirectory(const std::string& path);
    static std::string getFilename(const std::string& path);
    static std::string getStem(const std::string& path);
    static std::string getExtension(const std::string& path);
    static std::string join(const std::string& path1, const std::string& path2);
    static std::string normalize(const std::string& path);
    
    // 目录操作
    static std::vector<std::string> listDirectory(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& path);
    static std::vector<std::string> listDirectories(const std::string& path);
    static std::vector<std::string> findFiles(const std::string& path, const std::string& pattern);
    static std::vector<std::string> findDirectories(const std::string& path, const std::string& pattern);
    
    // 文件信息
    static size_t getFileSize(const std::string& path);
    static std::string getFileHash(const std::string& path);
    static time_t getLastModified(const std::string& path);
    static time_t getLastAccessed(const std::string& path);
    static time_t getCreated(const std::string& path);
    
    // 权限操作
    static bool setPermissions(const std::string& path, int permissions);
    static int getPermissions(const std::string& path);
    static bool isReadable(const std::string& path);
    static bool isWritable(const std::string& path);
    static bool isExecutable(const std::string& path);
    
    // 临时文件
    static std::string createTempFile(const std::string& prefix = "chtl_");
    static std::string createTempDirectory(const std::string& prefix = "chtl_");
    static bool cleanupTempFiles();
    
    // 错误处理
    static std::string getLastError();
    static void clearLastError();
    
private:
    static std::string last_error_;
    static void setLastError(const std::string& error);
};

} // namespace CHTL

#endif // FILESYSTEM_H