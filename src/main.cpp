#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: chtl <input_file.chtl> [output_file.html]" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::string output_filename = "output.html";
    if (argc > 2) {
        output_filename = argv[2];
    }

    std::ifstream file(input_filename);
    if (!file) {
        std::cerr << "Error: Could not open file " << input_filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // --- Lexing ---
    CHTL::Lexer lexer(source);

    // --- Parsing ---
    CHTL::Parser parser(lexer);
    std::unique_ptr<CHTL::ProgramNode> program = parser.parseProgram();

    auto errors = parser.getErrors();
    if (!errors.empty()) {
        std::cerr << "Parser encountered " << errors.size() << " errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "- " << error << std::endl;
        }
        return 1;
    }

    if (!program) {
        std::cerr << "Error: Parsing failed and resulted in a null program." << std::endl;
        return 1;
    }

    // --- Generation ---
    CHTL::Generator generator;
    std::string html_output = generator.generate(*program);

    // --- Output ---
    std::ofstream output_file(output_filename);
    if (!output_file) {
        std::cerr << "Error: Could not open output file " << output_filename << std::endl;
        return 1;
    }

    output_file << html_output;
    std::cout << "Successfully compiled " << input_filename << " to " << output_filename << std::endl;

    return 0;
}
