#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static std::string readFile(const std::string& filename);
    static bool writeFile(const std::string& filename, const std::string& content);
    static std::vector<std::string> listFiles(const std::string& directory);
    static std::vector<std::string> listDirectories(const std::string& directory);
    static bool createDirectory(const std::string& path);
    static bool removeFile(const std::string& filename);
    static bool removeDirectory(const std::string& path);
    static std::string getCurrentDirectory();
    static std::string getAbsolutePath(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string joinPath(const std::string& path1, const std::string& path2);
};

} // namespace CHTL

#endif // FILE_SYSTEM_H