#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <memory>

namespace CHTL {

// Forward declare to avoid circular dependency
struct ParserContext;

class CHTLLoader {
public:
    CHTLLoader() = default;

    // Returns the file content, or std::nullopt if the file has already been included
    // or cannot be read.
    std::optional<std::string> loadFile(const std::string& relative_path, const std::string& base_path);

    // Loads a CJMOD shared library.
    void loadCJMOD(const std::string& module_path, const std::string& base_path, std::shared_ptr<ParserContext> context);

private:
    std::vector<std::string> included_files_;
    std::vector<std::string> loaded_cjmods_;
};

} // namespace CHTL

#endif // CHTL_LOADER_H
