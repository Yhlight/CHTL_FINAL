#include "ZipUtil.h"
#include "../FileSystem/FileSystem.h"
#include "../../miniz.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

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

    mz_zip_archive zip_archive = {};
    mz_bool status = mz_zip_writer_init_file(&zip_archive, zip_path.c_str(), 0);
    if (!status) {
        std::cerr << "Error: Could not initialize zip archive '" << zip_path << "'." << std::endl;
        return false;
    }

    for (const auto& relative_path : files_to_add) {
        std::string full_path = directory_path + "/" + relative_path;
        std::string file_content = FileSystem::readFile(full_path);
        if (!file_content.empty()) {
            status = mz_zip_writer_add_mem(&zip_archive, relative_path.c_str(), file_content.c_str(), file_content.length(), MZ_DEFAULT_COMPRESSION);
            if (!status) {
                std::cerr << "Error: Could not add file '" << relative_path << "' to zip." << std::endl;
                mz_zip_writer_end(&zip_archive);
                return false;
            }
        }
    }

    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);
    return true;
}


// --- unpackToMemory Implementation ---
std::map<std::string, std::string> ZipUtil::unpackToMemory(const std::string& zip_path) {
    std::map<std::string, std::string> file_contents;
    mz_zip_archive zip_archive = {};

    mz_bool status = mz_zip_reader_init_file(&zip_archive, zip_path.c_str(), 0);
    if (!status) {
        std::cerr << "Error: Could not open zip archive '" << zip_path << "'." << std::endl;
        return file_contents;
    }

    int file_count = mz_zip_reader_get_num_files(&zip_archive);
    for (int i = 0; i < file_count; ++i) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            continue;
        }

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue; // Skip directories
        }

        size_t uncompressed_size = file_stat.m_uncomp_size;
        std::unique_ptr<char[]> buffer(new char[uncompressed_size]);

        if (mz_zip_reader_extract_to_mem(&zip_archive, i, buffer.get(), uncompressed_size, 0)) {
            file_contents[file_stat.m_filename] = std::string(buffer.get(), uncompressed_size);
        }
    }

    mz_zip_reader_end(&zip_archive);
    return file_contents;
}


} // namespace Util
} // namespace CHTL
