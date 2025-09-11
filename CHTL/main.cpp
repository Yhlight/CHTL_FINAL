#include <iostream>
#include <string>
#include <vector>
#include "CompilerDispatcher/CompilerDispatcher.h"

struct Args {
    std::string entry_filepath;
    bool default_struct = false;
};

Args parseArgs(int argc, char* argv[]) {
    Args args;
    std::vector<std::string> raw_args(argv + 1, argv + argc);
    for (const auto& arg : raw_args) {
        if (arg == "--default-struct") {
            args.default_struct = true;
        } else if (args.entry_filepath.empty()) {
            args.entry_filepath = arg;
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <entry_filename> [--default-struct]" << std::endl;
        return 1;
    }

    Args args = parseArgs(argc, argv);
    if (args.entry_filepath.empty()) {
        std::cerr << "Error: No entry filename provided." << std::endl;
        return 1;
    }

    CHTL::CompilerDispatcher dispatcher;
    std::string output = dispatcher.compile(args.entry_filepath, args.default_struct);

    std::cout << "--- CHTL Compiler Output ---" << std::endl;
    std::cout << output << std::endl;
    std::cout << "----------------------------" << std::endl;

    return 0;
}
