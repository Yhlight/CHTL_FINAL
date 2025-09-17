#pragma once

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    // Reads the entire content of a file into a string
    static std::string readFile(const std::string& path);

    // Writes a string to a file
    static void writeFile(const std::string& path, const std::string& content);

    // Checks if a file exists
    static bool fileExists(const std::string& path);

    // Extracts the directory part from a full path
    static std::string getDirectory(const std::string& path);
};

} // namespace CHTL
