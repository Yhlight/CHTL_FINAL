#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>

namespace CHTL {

class CHTLLoader {
public:
    // Resolves and loads the content of an imported file.
    // `importing_file_path` is the full path of the file containing the import statement.
    // `import_path` is the path string from the import statement itself (e.g., "./lib.chtl").
    static std::string load(const std::string& importing_file_path, const std::string& import_path);
};

} // namespace CHTL

#endif // CHTL_LOADER_H
