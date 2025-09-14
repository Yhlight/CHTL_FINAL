#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {

std::optional<std::string> CHTLLoader::loadFile(const std::string& relative_path, const std::string& base_path) {
    std::vector<std::filesystem::path> search_paths;

    // 1. Base path (usually the directory of the file doing the importing)
    std::filesystem::path base_dir = base_path.empty() ? "." : std::filesystem::path(base_path).parent_path();

    // The file could be in the same directory as the importer.
    search_paths.push_back(base_dir);

    // It could also be in a 'module' subdirectory relative to the importer's location.
    search_paths.push_back(base_dir / "module");

    // Or it could be in a 'module' directory at the project root (where the compiler is run from)
    search_paths.push_back(std::filesystem::current_path() / "tests" / "module");

    // 3. TODO: Add official compiler module directory
    // For example: std::filesystem::path executable_path = ...;
    // search_paths.push_back(executable_path.parent_path() / "module");

    std::filesystem::path found_path;
    bool file_exists = false;

    for (const auto& search_dir : search_paths) {
        auto potential_path = search_dir / relative_path;
        if (std::filesystem::exists(potential_path)) {
            found_path = potential_path;
            file_exists = true;
            break;
        }
    }

    if (!file_exists) {
        std::cerr << "Error: Could not find file " << relative_path << " in any search path." << std::endl;
        return std::nullopt;
    }

    try {
        // Get the canonical path to handle relative paths and symlinks consistently.
        std::string canonical_path = std::filesystem::canonical(found_path).string();

        // Check if the file has already been included.
        auto it = std::find(included_files_.begin(), included_files_.end(), canonical_path);
        if (it != included_files_.end()) {
            // Already processed, return nullopt to signal the parser to skip.
            return std::nullopt;
        }

        std::ifstream file(canonical_path);
        if (!file.is_open()) {
            // This check is somewhat redundant due to `filesystem::exists` above, but good for safety.
            std::cerr << "Error: Could not open file " << canonical_path << std::endl;
            return std::nullopt;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        // Mark this file as included.
        included_files_.push_back(canonical_path);

        return buffer.str();

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem Error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

} // namespace CHTL
