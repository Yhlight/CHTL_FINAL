#pragma once

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
    static std::vector<std::string> listDirectory(const std::string& path);
};

} // namespace CHTL
