#include <iostream>
#include <filesystem>
#include "CHTL/Compiler.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    try {
        std::filesystem::path entry_path(argv[1]);
        // The base directory for all relative imports will be the directory
        // of the main entry file.
        std::string base_dir = entry_path.parent_path().string();
        if (base_dir.empty()) {
            base_dir = ".";
        }

        CHTL::Compiler compiler(base_dir);
        std::string html = compiler.compile(entry_path.string());
        std::cout << html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
