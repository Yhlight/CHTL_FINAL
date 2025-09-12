#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace CHTL {

std::optional<std::string> CHTLLoader::loadFile(const std::string& relative_path, const std::string& base_path) {
    try {
        std::filesystem::path full_path;
        if (!base_path.empty()) {
            full_path = std::filesystem::path(base_path).parent_path() / relative_path;
        } else {
            full_path = relative_path;
        }

        // Get the canonical path to handle relative paths and symlinks consistently.
        std::string canonical_path = std::filesystem::canonical(full_path).string();

        // Check if the file has already been included.
        auto it = std::find(included_files_.begin(), included_files_.end(), canonical_path);
        if (it != included_files_.end()) {
            // Already processed, return nullopt to signal the parser to skip.
            return std::nullopt;
        }

        std::ifstream file(canonical_path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << relative_path << std::endl;
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
