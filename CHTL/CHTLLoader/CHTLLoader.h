#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>

namespace CHTL {

class CHTLLoader {
public:
    static std::string load(const std::string& importing_file_path, const std::string& import_path);
};

} // namespace CHTL

#endif // CHTL_LOADER_H
