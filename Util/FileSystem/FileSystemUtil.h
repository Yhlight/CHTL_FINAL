#ifndef CHTL_FILESYSTEMUTIL_H
#define CHTL_FILESYSTEMUTIL_H

#include <string>

namespace FileSystemUtil {

    // Reads the entire content of a file into a string
    std::string readFile(const std::string& filePath);

    // Writes a string to a file, overwriting it if it exists
    bool writeFile(const std::string& filePath, const std::string& content);

} // namespace FileSystemUtil

#endif //CHTL_FILESYSTEMUTIL_H
