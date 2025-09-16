#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif

namespace CHTL {

// Helper function to get the directory of the current executable
std::filesystem::path get_executable_directory() {
#ifdef _WIN32
    char path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count > 0) {
        return std::filesystem::path(result).parent_path();
    }
    return ""; // Fallback
#endif
}

std::optional<std::string> CHTLLoader::loadFile(const std::string& import_path, const std::string& base_path_str) {
    std::filesystem::path base_path(base_path_str);
    std::filesystem::path path_obj(import_path);

    std::vector<std::filesystem::path> search_paths;
    // 1. Official Modules Path (next to executable)
    search_paths.push_back(get_executable_directory() / "module");
    // 2. User Modules Path (relative to the file doing the import)
    if (!base_path.empty()) {
       search_paths.push_back(base_path.parent_path() / "module");
    }
    // 3. Local Path (relative to the file doing the import)
    if (!base_path.empty()) {
        search_paths.push_back(base_path.parent_path());
    }
    // 4. Current working directory as a fallback
    search_paths.push_back(std::filesystem::current_path());

    for (const auto& search_dir : search_paths) {
        std::filesystem::path potential_file = search_dir / path_obj;

        if (!std::filesystem::exists(potential_file)) {
            continue;
        }

        try {
            std::string canonical_path = std::filesystem::canonical(potential_file).string();
            if (std::find(included_files_.begin(), included_files_.end(), canonical_path) != included_files_.end()) {
                return std::nullopt; // Circular dependency
            }

            if (potential_file.extension() == ".chtl") {
                std::ifstream file(canonical_path);
                if (!file.is_open()) {
                    continue;
                }
                std::stringstream buffer;
                buffer << file.rdbuf();
                included_files_.push_back(canonical_path);
                return buffer.str();
            }

        } catch (const std::filesystem::filesystem_error& e) {
            continue;
        }
    }

    std::cerr << "Error: Could not find module or file: " << import_path << std::endl;
    return std::nullopt;
}

} // namespace CHTL
