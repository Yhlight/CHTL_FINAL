#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

std::optional<std::string> CHTLLoader::loadFile(const std::string& relative_path, const std::string& base_path) {
    try {
        // Create a path object for the base file
        std::filesystem::path base_file_path(base_path);
        // Get the directory containing the base file
        std::filesystem::path base_dir = base_file_path.parent_path();
        // Combine the base directory with the relative path of the new file
        std::filesystem::path target_file_path = base_dir / relative_path;

        // Get the canonical (absolute, normalized) path to prevent duplicates
        std::filesystem::path canonical_path = std::filesystem::canonical(target_file_path);
        std::string canonical_path_str = canonical_path.string();

        // Check for circular dependencies or duplicate includes
        for (const auto& included : included_files_) {
            if (included == canonical_path_str) {
                // File already included, return nullopt to signify skipping.
                return std::nullopt;
            }
        }

        // Add the file to the list of included files
        included_files_.push_back(canonical_path_str);

        // Open and read the file
        std::ifstream file_stream(canonical_path);
        if (!file_stream) {
            std::cerr << "Warning: Could not open file for import: " << canonical_path_str << std::endl;
            return std::nullopt;
        }

        std::stringstream buffer;
        buffer << file_stream.rdbuf();
        return buffer.str();

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Warning: Filesystem error while trying to load '" << relative_path << "' from '" << base_path << "': " << e.what() << std::endl;
        return std::nullopt;
    }
}

} // namespace CHTL
