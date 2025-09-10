#ifndef CHTL_FILESYSTEM_H
#define CHTL_FILESYSTEM_H

#include <string>

namespace FileSystem {
    // Reads the entire content of a file into a string.
    // Returns an empty string if the file cannot be read.
    std::string readFile(const std::string& path);

    // Writes a string to a file.
    bool writeFile(const std::string& path, const std::string& content);

    // Checks if a file exists.
    bool fileExists(const std::string& path);

} // namespace FileSystem

#endif // CHTL_FILESYSTEM_H
