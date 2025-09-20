#include "CompilerDispatcher.h"
#include "Config/Configuration.h"
#include "Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <vector>

void print_usage() {
    std::cout << "Usage: chtl <input_file> [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -o <output_name>   Specify the base name for output files (e.g., 'index' -> index.html).\n";
    std::cout << "  --inline           Output a single self-contained HTML file.\n";
    std::cout << "  --help             Show this help message.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string input_file;
    std::string output_base_name = "output";
    bool inline_output = false;

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--help") {
            print_usage();
            return 0;
        }
        if (args[i] == "--inline") {
            inline_output = true;
        } else if (args[i] == "-o") {
            if (i + 1 < args.size()) {
                output_base_name = args[++i];
            } else {
                std::cerr << "Error: -o option requires a value." << std::endl;
                return 1;
            }
        } else {
            if (input_file.empty()) {
                input_file = args[i];
            } else {
                std::cerr << "Error: Multiple input files specified. Only one is allowed." << std::endl;
                return 1;
            }
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        return 1;
    }

    std::string source = CHTL::FileSystem::readFile(input_file);
    if (source.empty()) {
        std::cerr << "Error: Could not read input file '" << input_file << "'." << std::endl;
        return 1;
    }

    std::cout << "Compiling " << input_file << "..." << std::endl;

    try {
        auto config = std::make_shared<CHTL::Configuration>();
        CHTL::CompilerDispatcher dispatcher(config, input_file);
        CHTL::FinalCompilationResult result = dispatcher.dispatch(source);

        if (inline_output) {
            std::string final_html = "<!DOCTYPE html>\n<html>\n<head>\n<style>\n" + result.css + "\n</style>\n</head>\n<body>\n" + result.html + "\n<script>\n" + result.js + "\n</script>\n</body>\n</html>";
            CHTL::FileSystem::writeFile(output_base_name + ".html", final_html);
            std::cout << "Successfully generated inlined file: " << output_base_name << ".html" << std::endl;
        } else {
            CHTL::FileSystem::writeFile(output_base_name + ".html", result.html);
            CHTL::FileSystem::writeFile(output_base_name + ".css", result.css);
            CHTL::FileSystem::writeFile(output_base_name + ".js", result.js);
            std::cout << "Successfully generated files: " << output_base_name << ".html, .css, .js" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
