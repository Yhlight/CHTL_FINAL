#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include "../CHTLParser/ParserContext.h"

namespace CHTL {

class CHTLLoader {
public:
    CHTLLoader() = default;

    // Returns the file content, or std::nullopt if the file has already been included
    // or cannot be read.
    std::optional<std::string> loadFile(const std::string& relative_path, const std::string& base_path);
    void loadModule(const std::string& module_name, std::shared_ptr<ParserContext> context);

private:
    std::vector<std::string> included_files_;
    std::vector<std::string> loaded_modules_;
};

} // namespace CHTL

#endif // CHTL_LOADER_H
