#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>

namespace CHTL {

class FileSystem {
public:
    // Reads the entire content of a file into a string
    static std::string readFile(const std::string& path);
    // Writes a string to a file
    static bool writeFile(const std::string& path, const std::string& content);
};

} // namespace CHTL

#endif // FILE_SYSTEM_H
