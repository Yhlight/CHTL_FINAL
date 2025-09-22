#include "ZipUtil.h"
#include "../FileSystem/FileSystem.h"
#include <zip.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <filesystem>
#include <cstring>

namespace CHTL {
namespace Util {

// --- packDirectory Implementation ---
// Helper function to recursively list files
void listFilesRecursive(const std::string& path, std::vector<std::string>& files, const std::string& base_path) {
    for (const auto& entry : FileSystem::listDirectory(path)) {
        std::string full_path = path + "/" + entry;
        if (FileSystem::isDirectory(full_path)) {
            listFilesRecursive(full_path, files, base_path);
        } else {
            // Store the path relative to the base directory
            files.push_back(full_path.substr(base_path.length() + 1));
        }
    }
}

bool ZipUtil::packDirectory(const std::string& directory_path, const std::string& zip_path) {
    std::vector<std::string> files_to_add;
    // We pass directory_path as the base_path to make all archived paths relative
    listFilesRecursive(directory_path, files_to_add, directory_path);

    int error = 0;
    zip_t* zip_archive = zip_open(zip_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!zip_archive) {
        std::cerr << "Error: Could not create zip archive '" << zip_path << "'." << std::endl;
        return false;
    }

    for (const auto& relative_path : files_to_add) {
        std::string full_path = directory_path + "/" + relative_path;
        std::string file_content = FileSystem::readFile(full_path);
        if (!file_content.empty()) {
            zip_source_t* source = zip_source_buffer(zip_archive, file_content.c_str(), file_content.length(), 0);
            if (!source) {
                std::cerr << "Error: Could not create zip source for '" << relative_path << "'." << std::endl;
                zip_close(zip_archive);
                return false;
            }
            
            if (zip_file_add(zip_archive, relative_path.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
                std::cerr << "Error: Could not add file '" << relative_path << "' to zip." << std::endl;
                zip_source_free(source);
                zip_close(zip_archive);
                return false;
            }
        }
    }

    zip_close(zip_archive);
    return true;
}


// --- unpackToMemory Implementation ---
std::map<std::string, std::string> ZipUtil::unpackToMemory(const std::string& zip_path) {
    std::map<std::string, std::string> result;
    
    int error = 0;
    zip_t* zip_archive = zip_open(zip_path.c_str(), ZIP_RDONLY, &error);
    
    if (!zip_archive) {
        std::cerr << "Error: Could not open zip archive '" << zip_path << "'." << std::endl;
        return result;
    }

    zip_int64_t num_entries = zip_get_num_entries(zip_archive, 0);
    
    for (zip_int64_t i = 0; i < num_entries; i++) {
        zip_stat_t stat;
        if (zip_stat_index(zip_archive, i, 0, &stat) == 0) {
            // Skip directories
            if (stat.name[strlen(stat.name) - 1] == '/') {
                continue;
            }
            
            zip_file_t* file = zip_fopen_index(zip_archive, i, 0);
            if (file) {
                std::vector<char> buffer(stat.size);
                zip_int64_t bytes_read = zip_fread(file, buffer.data(), stat.size);
                if (bytes_read == stat.size) {
                    result[stat.name] = std::string(buffer.data(), buffer.size());
                }
                zip_fclose(file);
            }
        }
    }
    
    zip_close(zip_archive);
    return result;
}

} // namespace Util
} // namespace CHTL