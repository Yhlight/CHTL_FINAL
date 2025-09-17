#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    // Read entire file into a string
    static bool ReadFile(const std::string& path, std::string& content);

    // Write a string to a file
    static bool WriteFile(const std::string& path, const std::string& content);

    // Check if a file exists
    static bool FileExists(const std::string& path);
};

} // namespace CHTL

#endif // FILESYSTEM_H
