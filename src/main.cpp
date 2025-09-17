#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [output_file.html]" << std::endl;
        return 1;
    }

    std::string input_filepath = argv[1];
    std::string output_filepath = (argc > 2) ? argv[2] : "output.html";

    std::ifstream file(input_filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << input_filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    if (source.empty()) {
        std::cout << "Input file is empty. Nothing to generate." << std::endl;
        return 0;
    }

    try {
        // 1. Scan the source code to get fragments
        CHTL::CHTLUnifiedScanner scanner(source);
        std::vector<CHTL::CodeFragment> fragments = scanner.scan();

        // 2. Dispatch fragments to their respective compilers
        CHTL::CompilerDispatcher dispatcher(input_filepath, scanner.getPlaceholderMap());
        CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

        // 3. Write the final, merged result to the output file
        std::ofstream html_file(output_filepath);
        if (!html_file.is_open()) {
            std::cerr << "Error: Could not open " << output_filepath << " for writing." << std::endl;
            return 1;
        }
        html_file << result.html;
        std::cout << "Successfully generated " << output_filepath << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
