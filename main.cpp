#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "src/CHTL/Lexer/Lexer.h"
#include "src/CHTL/Parser/Parser.h"
#include "src/CHTL/Generator/Generator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file " << input_filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string source = buffer.str();

    try {
        // 1. Lexing
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.getAllTokens();

        // 2. Parsing
        CHTL::Parser parser(tokens);
        std::shared_ptr<CHTL::RootNode> ast = parser.parse();

        // 3. Generating
        CHTL::Generator generator(ast);
        generator.generate();
        std::string html_output = generator.getHtml();

        // 4. Writing output
        std::string output_filename = "output.html";
        std::ofstream output_file(output_filename);
        if (!output_file.is_open()) {
            std::cerr << "Error: Could not open output file " << output_filename << std::endl;
            return 1;
        }
        output_file << html_output;

        std::cout << "Successfully compiled " << input_filename << " to " << output_filename << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
