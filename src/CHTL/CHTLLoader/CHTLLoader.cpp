#include "CHTLLoader.h"
#include "../CHTLParser/ParserContext.h"
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
#include <dlfcn.h>
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
    search_paths.push_back(get_executable_directory() / "module");
    if (!base_path.empty()) {
       search_paths.push_back(base_path.parent_path() / "module");
       search_paths.push_back(base_path.parent_path());
    }
    search_paths.push_back(std::filesystem::current_path());

    for (const auto& search_dir : search_paths) {
        std::filesystem::path potential_file = search_dir / path_obj;

        if (std::filesystem::exists(potential_file) && potential_file.extension() == ".chtl") {
             try {
                std::string canonical_path = std::filesystem::canonical(potential_file).string();
                if (std::find(included_files_.begin(), included_files_.end(), canonical_path) != included_files_.end()) {
                    return std::nullopt; // Circular dependency
                }
                std::ifstream file(canonical_path);
                if (!file.is_open()) continue;
                std::stringstream buffer;
                buffer << file.rdbuf();
                included_files_.push_back(canonical_path);
                return buffer.str();
            } catch (const std::filesystem::filesystem_error& e) {
                continue;
            }
        }
    }

    std::cerr << "Error: Could not find CHTL file: " << import_path << std::endl;
    return std::nullopt;
}


void CHTLLoader::loadCJMOD(const std::string& module_name, const std::string& base_path_str, std::shared_ptr<ParserContext> context) {
    if (std::find(loaded_cjmods_.begin(), loaded_cjmods_.end(), module_name) != loaded_cjmods_.end()) {
        return; // Already loaded
    }

    std::filesystem::path base_path(base_path_str);
    std::vector<std::filesystem::path> search_paths;
    search_paths.push_back(get_executable_directory() / "module");
     if (!base_path.empty()) {
       search_paths.push_back(base_path.parent_path() / "module");
       search_paths.push_back(base_path.parent_path());
    }
    search_paths.push_back(std::filesystem::current_path());

    #ifdef _WIN32
        const std::string lib_extension = ".dll";
    #else
        const std::string lib_extension = ".so";
    #endif

    for (const auto& search_dir : search_paths) {
        std::filesystem::path potential_lib = search_dir / (module_name + lib_extension);
        if (std::filesystem::exists(potential_lib)) {
            #ifdef _WIN32
                HMODULE handle = LoadLibrary(potential_lib.string().c_str());
                if (!handle) {
                    std::cerr << "Warning: Failed to load CJMOD library " << potential_lib << std::endl;
                    continue;
                }

                using InitFunc = void(*)(std::shared_ptr<CJMOD::CJMODManager>);
                InitFunc init_func = (InitFunc)GetProcAddress(handle, "chtl_cjmod_init");

                if (!init_func) {
                    std::cerr << "Warning: Could not find entry point 'chtl_cjmod_init' in " << potential_lib << std::endl;
                    FreeLibrary(handle);
                    continue;
                }
            #else // Linux
                void* handle = dlopen(potential_lib.string().c_str(), RTLD_LAZY);
                if (!handle) {
                    std::cerr << "Warning: Failed to load CJMOD library " << potential_lib << ": " << dlerror() << std::endl;
                    continue;
                }

                using InitFunc = void(*)(std::shared_ptr<CJMOD::CJMODManager>);
                InitFunc init_func = (InitFunc)dlsym(handle, "chtl_cjmod_init");

                char* error = dlerror();
                if (error != NULL) {
                    std::cerr << "Warning: Could not find entry point 'chtl_cjmod_init' in " << potential_lib << ": " << error << std::endl;
                    dlclose(handle);
                    continue;
                }
            #endif

            // Call the initialization function
            init_func(context->cjmod_manager);
            loaded_cjmods_.push_back(module_name);
            std::cout << "Successfully loaded CJMOD: " << module_name << std::endl;
            return; // Found and loaded successfully
        }
    }

    throw std::runtime_error("Could not find CJMOD: " + module_name);
}

} // namespace CHTL
