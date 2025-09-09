#ifndef FILESYSTEM_UTIL_H
#define FILESYSTEM_UTIL_H

#include <string>
#include <vector>

class FileSystem {
public:
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
    static std::vector<std::string> listDirectory(const std::string& path);
};

#endif // FILESYSTEM_UTIL_H
