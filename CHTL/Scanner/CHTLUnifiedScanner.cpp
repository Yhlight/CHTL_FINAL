#include "CHTLUnifiedScanner.h"
#include <filesystem>

namespace CHTL {

CodeFragment CHTLUnifiedScanner::scanFile(const std::string& filepath, const std::string& source) {
    std::string extension = std::filesystem::path(filepath).extension().string();

    FragmentType type = FragmentType::UNKNOWN;

    if (extension == ".chtl") {
        type = FragmentType::CHTL;
    } else if (extension == ".cjjs") {
        type = FragmentType::CHTL_JS;
    } else if (extension == ".css") {
        type = FragmentType::CSS;
    } else if (extension == ".js") {
        type = FragmentType::JS;
    }

    return {type, source};
}

} // namespace CHTL
