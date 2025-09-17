#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

namespace FileSystem {

    // Read the entire content of a file into a string
    // Throws a runtime_error on failure
    std::string readFile(const std::string& path);

    // Write a string to a file
    // Throws a runtime_error on failure
    void writeFile(const std::string& path, const std::string& content);

    // Check if a file or directory exists
    bool exists(const std::string& path);

    // List all files and directories in a given path
    // Throws a runtime_error on failure
    std::vector<std::string> listDirectory(const std::string& path);

    // Check if a path is a directory
    bool isDirectory(const std::string& path);

} // namespace FileSystem

#endif // FILE_SYSTEM_H
