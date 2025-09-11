#ifndef CHTL_FILE_SYSTEM_H
#define CHTL_FILE_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

class FileSystem {
private:
    std::string basePath;
    std::vector<std::string> errors;
    bool debugMode;

public:
    FileSystem();
    FileSystem(const std::string& basePath);
    virtual ~FileSystem() = default;
    
    // 基本属性
    const std::string& getBasePath() const { return basePath; }
    void setBasePath(const std::string& path) { basePath = path; }
    
    // 文件操作
    bool fileExists(const std::string& filename) const;
    bool exists(const std::string& path) const;
    bool createFile(const std::string& filename, const std::string& content = "");
    bool deleteFile(const std::string& filename);
    bool copyFile(const std::string& source, const std::string& destination);
    bool moveFile(const std::string& source, const std::string& destination);
    std::string readFile(const std::string& filename) const;
    bool writeFile(const std::string& filename, const std::string& content);
    bool appendFile(const std::string& filename, const std::string& content);
    
    // 目录操作
    bool directoryExists(const std::string& dirname) const;
    bool createDirectory(const std::string& dirname);
    bool deleteDirectory(const std::string& dirname);
    bool copyDirectory(const std::string& source, const std::string& destination);
    bool moveDirectory(const std::string& source, const std::string& destination);
    std::vector<std::string> listDirectory(const std::string& dirname) const;
    std::vector<std::string> listFiles(const std::string& dirname) const;
    std::vector<std::string> listDirectories(const std::string& dirname) const;
    
    // 路径操作
    std::string resolvePath(const std::string& path) const;
    std::string getFileName(const std::string& path) const;
    std::string getDirectory(const std::string& path) const;
    std::string getFileExtension(const std::string& path) const;
    std::string getBaseName(const std::string& path) const;
    bool isAbsolutePath(const std::string& path) const;
    bool isRelativePath(const std::string& path) const;
    
    // 错误管理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors();
    
    // 模式设置
    bool isDebugMode() const { return debugMode; }
    void setDebugMode(bool mode) { debugMode = mode; }
    
    // 工具方法
    std::string getCurrentDirectory() const;
    bool changeDirectory(const std::string& path);
    std::string getAbsolutePath(const std::string& path) const;
    bool isReadable(const std::string& path) const;
    bool isWritable(const std::string& path) const;
    bool isExecutable(const std::string& path) const;
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 克隆
    FileSystem clone() const;
    
    // 比较
    bool operator==(const FileSystem& other) const;
    bool operator!=(const FileSystem& other) const;
};

} // namespace CHTL

#endif // CHTL_FILE_SYSTEM_H