#include "CHTLLoader.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLLoader::load(const std::string& importing_file_path, const std::string& import_path) {
    std::string base_dir = FileSystem::getDirectory(importing_file_path);

    // 1. Try to load relative to the current file's directory.
    std::string relative_path = base_dir + import_path;
    if (FileSystem::fileExists(relative_path)) {
        return FileSystem::readFile(relative_path);
    }

    // 2. Try to load from the 'module' subdirectory relative to the current file.
    std::string module_path = base_dir + "module/" + import_path;
    if (FileSystem::fileExists(module_path)) {
        return FileSystem::readFile(module_path);
    }

    // If not found in any of the search paths, throw an error.
    throw std::runtime_error("Failed to load imported file: '" + import_path + "' not found in search paths.");
}

} // namespace CHTL
