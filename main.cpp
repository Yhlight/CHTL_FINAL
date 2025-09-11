#include <iostream>
#include <filesystem>
#include "CHTL/Compiler.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    try {
        CHTL::Compiler compiler( (std::filesystem::path(argv[1]).parent_path().string()) );
        std::string html = compiler.compile(argv[1]);
        std::cout << html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
