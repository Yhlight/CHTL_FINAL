#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // Step 1: Lexer
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.Tokenize();

        // Step 2: Parser
        CHTL::CHTLParser parser(tokens);
        CHTL::NodeList ast = parser.Parse();

        // Step 3: Generator
        CHTL::CHTLGenerator generator;
        std::string html_output = generator.Generate(ast);

        // Step 4: Print the output
        std::cout << html_output << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
