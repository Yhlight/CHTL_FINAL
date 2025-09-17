#include "CHTLLoader.h"
#include "../CHTLParser/ParserContext.h"
#include "../CHTLParser/CHTLParser.h" // For parsing CMOD info file
#include "../CHTLLexer/CHTLLexer.h"   // For parsing CMOD info file
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "miniz.h"

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

std::optional<std::string> CHTLLoader::loadFile(const std::string& import_path, const std::string& base_path_str, std::shared_ptr<ParserContext> context) {
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
        std::filesystem::path potential_cmod_file = search_dir / path_obj.replace_extension(".cmod");

        if (std::filesystem::exists(potential_cmod_file)) {
            // CMOD file takes precedence
            return loadCmodArchive(potential_cmod_file, base_path_str, context);
        }
        else if (std::filesystem::exists(potential_file) && potential_file.extension() == ".chtl") {
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

std::optional<std::string> CHTLLoader::loadCmodArchive(const std::filesystem::path& cmod_path, const std::string& base_path, std::shared_ptr<ParserContext> context) {
    try {
        std::string canonical_path = std::filesystem::canonical(cmod_path).string();
        if (std::find(loaded_cmods_.begin(), loaded_cmods_.end(), canonical_path) != loaded_cmods_.end()) {
            return std::nullopt; // Already loaded
        }
        loaded_cmods_.push_back(canonical_path);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Warning: Could not get canonical path for CMOD: " << cmod_path << " (" << e.what() << ")" << std::endl;
    }

    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    if (!mz_zip_reader_init_file(&zip_archive, cmod_path.string().c_str(), 0)) {
        std::cerr << "Warning: Could not open CMOD archive: " << cmod_path << std::endl;
        return std::nullopt;
    }

    std::string module_name = cmod_path.stem().string();

    // --- Step 1: Parse Info File to find out what to load ---
    std::string info_file_path_in_zip = module_name + "/info/" + module_name + ".chtl";
    int info_file_index = mz_zip_reader_locate_file(&zip_archive, info_file_path_in_zip.c_str(), nullptr, 0);
    if (info_file_index >= 0) {
        size_t uncompressed_size;
        void* p_uncompressed_data = mz_zip_reader_extract_to_heap(&zip_archive, info_file_index, &uncompressed_size, 0);
        if (p_uncompressed_data) {
            std::string info_content(static_cast<char*>(p_uncompressed_data), uncompressed_size);
            mz_free(p_uncompressed_data);

            CHTLParser info_parser(info_content, *this, cmod_path.string(), context);
            info_parser.current_namespace_ = module_name;
            info_parser.parse();
        }
    } else {
         std::cerr << "Warning: Could not find info file '" << info_file_path_in_zip << "' in CMOD archive: " << cmod_path << std::endl;
    }

    // --- Step 2: Pre-load all exported templates ---
    if (context->cmod_exports.count(module_name)) {
        auto exports_to_load = context->cmod_exports.at(module_name); // Make a copy
        for (const auto& export_item : exports_to_load) {
            std::string internal_path = module_name + "/" + export_item.source_file;
            std::string canonical_internal_path = cmod_path.string() + "::" + internal_path;

            if (std::find(included_files_.begin(), included_files_.end(), canonical_internal_path) != included_files_.end()) {
                continue; // Already parsed
            }

            int file_index = mz_zip_reader_locate_file(&zip_archive, internal_path.c_str(), nullptr, 0);
            if (file_index < 0) {
                std::cerr << "Warning: Could not find exported file '" << internal_path << "' in CMOD archive: " << cmod_path << std::endl;
                continue;
            }

            size_t uncompressed_size;
            void* p_uncompressed_data = mz_zip_reader_extract_to_heap(&zip_archive, file_index, &uncompressed_size, 0);
            if (!p_uncompressed_data) {
                std::cerr << "Warning: Failed to extract exported file '" << internal_path << "' from CMOD archive." << std::endl;
                continue;
            }

            std::string template_content(static_cast<char*>(p_uncompressed_data), uncompressed_size);
            mz_free(p_uncompressed_data);

            CHTLParser template_parser(template_content, *this, cmod_path.string(), context);
            template_parser.current_namespace_ = module_name;
            template_parser.parse();

            included_files_.push_back(canonical_internal_path);
        }
    }


    // --- Step 3: Extract and return the Main CHTL File ---
    std::string main_file_path_in_zip = module_name + "/" + module_name + ".chtl";
    int main_file_index = mz_zip_reader_locate_file(&zip_archive, main_file_path_in_zip.c_str(), nullptr, 0);
    if (main_file_index < 0) {
        std::cerr << "Error: Could not find main file '" << main_file_path_in_zip << "' in CMOD archive: " << cmod_path << std::endl;
        mz_zip_reader_end(&zip_archive);
        return std::nullopt;
    }

    size_t main_uncompressed_size;
    void* p_main_uncompressed_data = mz_zip_reader_extract_to_heap(&zip_archive, main_file_index, &main_uncompressed_size, 0);
    if (!p_main_uncompressed_data) {
        std::cerr << "Error: Failed to extract main file from CMOD archive: " << cmod_path << std::endl;
        mz_zip_reader_end(&zip_archive);
        return std::nullopt;
    }

    std::string main_content(static_cast<char*>(p_main_uncompressed_data), main_uncompressed_size);
    mz_free(p_main_uncompressed_data);
    mz_zip_reader_end(&zip_archive);

    included_files_.push_back(cmod_path.string() + "::" + main_file_path_in_zip);

    return main_content;
}

void CHTLLoader::loadSharedLibrary(const std::string& module_name, const std::string& base_path_str, std::shared_ptr<ParserContext> context) {
    if (std::find(loaded_shared_libs_.begin(), loaded_shared_libs_.end(), module_name) != loaded_shared_libs_.end()) {
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
            loaded_shared_libs_.push_back(module_name);
            std::cout << "Successfully loaded CJMOD: " << module_name << std::endl;
            return; // Found and loaded successfully
        }
    }

    throw std::runtime_error("Could not find CJMOD: " + module_name);
}

} // namespace CHTL
