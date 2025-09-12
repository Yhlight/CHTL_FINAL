#include "Compiler.h"
#include "CHTLParser.h" // For the ParseError
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: chtl <input_file> [output_file]" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string source = buffer.str();

    std::string html_output;
    try {
        html_output = compile_source(source);
    } catch (const CHTL::CHTLParser::ParseError& e) {
        // The parser already prints a detailed error message.
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    if (argc == 3) {
        std::ofstream outputFile(argv[2]);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open output file '" << argv[2] << "'" << std::endl;
            return 1;
        }
        outputFile << html_output;
        std::cout << "Successfully compiled '" << argv[1] << "' to '" << argv[2] << "'" << std::endl;
    } else {
        std::cout << html_output;
    }

    return 0;
}
