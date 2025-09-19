#include "CHTLLoader.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLLoader::load(const std::string& importing_file_path, const std::string& import_path) {
    std::string base_dir = FileSystem::getDirectory(importing_file_path);
    std::string full_path = base_dir + import_path;

    std::string content = FileSystem::readFile(full_path);
    if (content.empty()) {
        throw std::runtime_error("Failed to load imported file: " + full_path);
    }

    return content;
}

} // namespace CHTL
