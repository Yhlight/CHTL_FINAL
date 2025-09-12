#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>
#include <filesystem>

namespace CHTL {

class FileSystem {
public:
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static std::vector<std::string> listFiles(const std::string& path);
    static bool removeFile(const std::string& path);
    static bool removeDirectory(const std::string& path);
};

} // namespace CHTL

#endif // FILE_SYSTEM_H