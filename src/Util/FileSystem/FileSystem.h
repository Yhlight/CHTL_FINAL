#ifndef CHTL_FILESYSTEM_H
#define CHTL_FILESYSTEM_H

#include <string>

namespace CHTL {
namespace Util {

class FileSystem {
public:
    // Read an entire file into a string. Throws std::runtime_error on failure.
    static std::string readFile(const std::string& filepath);

    // Write a string to a file. Returns false on failure.
    static bool writeFile(const std::string& filepath, const std::string& content);
};

} // namespace Util
} // namespace CHTL

#endif // CHTL_FILESYSTEM_H
