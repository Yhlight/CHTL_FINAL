#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>

namespace CHTL {

class FileSystem {
public:
    static std::string readFile(const std::string& path);
};

} // namespace CHTL

#endif // FILE_SYSTEM_H
