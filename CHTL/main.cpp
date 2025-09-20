#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"
#include "../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <input_file.chtl> [options]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -o <output_base>   Set the base name for output files (default: 'output')" << std::endl;
    std::cerr << "  --inline             Output a single HTML file with inlined CSS and JS" << std::endl;
    std::cerr << "  --inline-css         Output an HTML file with inlined CSS" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string input_file;
    std::string output_base = "output";
    bool inline_output = false;
    bool inline_css = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) {
                output_base = argv[++i];
            } else {
                std::cerr << "Error: -o option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--inline") {
            inline_output = true;
        } else if (arg == "--inline-css") {
            inline_css = true;
        } else {
            if (!input_file.empty()) {
                std::cerr << "Error: More than one input file specified." << std::endl;
                print_usage(argv[0]);
                return 1;
            }
            input_file = arg;
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    try {
        std::cout << "Compiling " << input_file << "..." << std::endl;
        std::string source = CHTL::FileSystem::readFile(input_file);

        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput compiled_output = dispatcher.dispatch();

        CHTL::CodeMerger merger(compiled_output);
        if (inline_output) {
            std::string final_html = merger.mergeToSingleFile();
            std::string out_filename = output_base + ".html";
            std::ofstream out_file(out_filename);
            out_file << final_html;
            out_file.close();
            std::cout << "Successfully generated " << out_filename << std::endl;
        } else if (inline_css) {
            merger.saveCssInlined(output_base);
            std::cout << "Successfully generated " << output_base << ".html with inlined css." << std::endl;
        } else {
            merger.saveToSeparateFiles(output_base);
            std::cout << "Successfully generated " << output_base << ".html and related files." << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
