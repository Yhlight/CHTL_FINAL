#include <iostream>
#include <string>
#include <vector>
#include "../../Util/FileSystem/FileSystem.h"
#include "../../miniz.h"

void add_file_to_zip(mz_zip_archive* zip_archive, const std::string& file_path, const std::string& entry_name) {
    // This is a simplified implementation. A real one would handle errors.
    mz_zip_writer_add_file(zip_archive, entry_name.c_str(), file_path.c_str(), nullptr, 0, MZ_DEFAULT_COMPRESSION);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <module_directory>" << std::endl;
        return 1;
    }

    std::string module_dir = argv[1];
    if (!CHTL::FileSystem::isDirectory(module_dir)) {
        std::cerr << "Error: Provided path is not a valid directory." << std::endl;
        return 1;
    }

    // Validate structure
    std::string src_path = module_dir + "/src";
    std::string info_path = module_dir + "/info";
    if (!CHTL::FileSystem::isDirectory(src_path) || !CHTL::FileSystem::isDirectory(info_path)) {
        std::cerr << "Error: Module directory must contain 'src' and 'info' subdirectories." << std::endl;
        return 1;
    }

    std::string module_name = CHTL::FileSystem::getFilename(module_dir);
    std::string zip_filename = module_name + ".cmod";

    mz_zip_archive zip_archive = {};
    mz_zip_writer_init_file(&zip_archive, zip_filename.c_str(), 0);

    std::cout << "Creating " << zip_filename << "..." << std::endl;

    // Add files from src
    std::vector<std::string> src_files = CHTL::FileSystem::getFilesInDirectoryRecursive(src_path);
    for (const auto& file : src_files) {
        std::string entry_name = "src/" + CHTL::FileSystem::getFilename(file);
        std::cout << "  - Adding " << entry_name << std::endl;
        add_file_to_zip(&zip_archive, file, entry_name);
    }

    // Add files from info
    std::vector<std::string> info_files = CHTL::FileSystem::getFilesInDirectoryRecursive(info_path);
    for (const auto& file : info_files) {
        std::string entry_name = "info/" + CHTL::FileSystem::getFilename(file);
        std::cout << "  - Adding " << entry_name << std::endl;
        add_file_to_zip(&zip_archive, file, entry_name);
    }

    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);

    std::cout << "Module '" << module_name << "' successfully packaged as " << zip_filename << std::endl;

    return 0;
}
