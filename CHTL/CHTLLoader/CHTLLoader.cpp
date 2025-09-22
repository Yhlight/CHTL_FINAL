#include "CHTLLoader.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/ZipUtil/ZipUtil.h"
#include <stdexcept>
#include <vector>

namespace CHTL {

// Helper to get a filename without extension
std::string getBasename(const std::string& path) {
    size_t last_slash = path.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);
    size_t last_dot = filename.rfind('.');
    if (last_dot != std::string::npos) {
        return filename.substr(0, last_dot);
    }
    return filename;
}

std::string loadFileContent(const std::string& path) {
    if (path.size() > 5 && path.substr(path.size() - 5) == ".cmod") {
        auto unpacked_files = Util::ZipUtil::unpackToMemory(path);
        if (unpacked_files.empty()) {
            throw std::runtime_error("Failed to unpack CMOD file: " + path);
        }
        std::string module_name = getBasename(path);
        std::string main_src_path = "src/" + module_name + ".chtl";
        if (unpacked_files.count(main_src_path)) {
            return unpacked_files.at(main_src_path);
        }
        throw std::runtime_error("Could not find a main .chtl file inside CMOD: " + path);
    } else {
        return FileSystem::readFile(path);
    }
}


std::string CHTLLoader::load(const std::string& importing_file_path, const std::string& import_path) {
    std::string base_dir = FileSystem::getDirectory(importing_file_path);
    // For now, we'll mock the executable path. A real implementation would get this properly.
    std::string executable_dir = "./";

    std::vector<std::string> search_paths;

    // 1. Generate potential paths
    std::string official_module_dir = executable_dir + "module/";
    std::string user_module_dir = base_dir + "module/";

    std::vector<std::string> base_search_dirs;
    if (import_path.rfind("chtl::", 0) == 0) {
        // Only search official directory for chtl:: prefix
        base_search_dirs.push_back(official_module_dir);
    } else {
        base_search_dirs.push_back(official_module_dir);
        base_search_dirs.push_back(user_module_dir);
        base_search_dirs.push_back(base_dir);
    }

    std::string final_import_path = (import_path.rfind("chtl::", 0) == 0) ? import_path.substr(6) : import_path;

    bool has_extension = final_import_path.rfind('.') != std::string::npos;

    for (const auto& dir : base_search_dirs) {
        if (has_extension) {
            search_paths.push_back(dir + final_import_path);
        } else {
            // Prioritize .cmod over .chtl
            search_paths.push_back(dir + final_import_path + ".cmod");
            search_paths.push_back(dir + final_import_path + ".chtl");
        }
    }

    // 2. Iterate and load the first valid path
    for (const auto& path : search_paths) {
        if (FileSystem::fileExists(path)) {
            return loadFileContent(path);
        }
    }

    // If not found in any of the search paths, throw an error.
    throw std::runtime_error("Failed to load imported file: '" + import_path + "' not found in any search paths.");
}

} // namespace CHTL
