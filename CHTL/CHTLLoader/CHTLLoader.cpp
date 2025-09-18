#include "CHTLLoader.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLLoader::load(const std::string& importing_file_path, const std::string& import_path) {
    // For now, we only support simple relative paths.
    // A real implementation would check for module paths, etc.

    std::string base_dir = FileSystem::getDirectory(importing_file_path);
    std::string full_path = base_dir + import_path;

    // TODO: Add proper path canonicalization to handle ".." etc.

    std::string content = FileSystem::readFile(full_path);
    if (content.empty()) {
        // readFile already prints an error, but we can throw a more specific one.
        throw std::runtime_error("Failed to load imported file: " + full_path);
    }

    return content;
}

} // namespace CHTL
