#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>

#include <vector>

namespace CHTL {

class FileSystem {
public:
    static std::string readFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static std::string getFilename(const std::string& path);
    static std::vector<std::string> getFilesInDirectoryRecursive(const std::string& path);
};

} // namespace CHTL

#endif // FILE_SYSTEM_H
