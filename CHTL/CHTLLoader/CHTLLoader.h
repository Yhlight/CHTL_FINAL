#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>

namespace CHTL {

class CHTLLoader {
public:
    // Reads the entire content of a file into a string.
    // Throws std::runtime_error if the file cannot be opened.
    std::string load(const std::string& path);
};

} // namespace CHTL

#endif // CHTL_LOADER_H
