#pragma once

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    // Read entire file into a string
    static std::string readFile(const std::string& path);

    // Write a string to a file
    static bool writeFile(const std::string& path, const std::string& content);
};

} // namespace CHTL
