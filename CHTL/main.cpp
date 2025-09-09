#include <iostream>
#include <string>
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "Util/FileSystem/FileSystem.h"

std::string getOutputFilename(const std::string& input_filename) {
    size_t last_dot = input_filename.find_last_of(".");
    if (last_dot == std::string::npos) {
        return input_filename + ".html";
    }
    return input_filename.substr(0, last_dot) + ".html";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::string source;
    try {
        source = CHTL::FileSystem::readFile(input_filename);
    } catch (const std::exception& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return 1;
    }

    CHTL::CompilerDispatcher dispatcher;
    std::string final_html = dispatcher.compile(source);

    // A simple check if compilation failed (e.g., parser errors)
    if (final_html.rfind("Parser errors:", 0) == 0) {
        std::cerr << final_html << std::endl;
        return 1;
    }

    std::string output_filename = getOutputFilename(input_filename);
    try {
        CHTL::FileSystem::writeFile(output_filename, final_html);
    } catch (const std::exception& e) {
        std::cerr << "Error writing file: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Successfully compiled " << input_filename << " to " << output_filename << std::endl;

    return 0;
}
