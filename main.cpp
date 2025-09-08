#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext.h"
#include <memory>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file.chtl>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // 1. Pre-scan for configuration
        auto config = std::make_shared<ConfigurationState>();
        Lexer pre_lexer(source, config);
        Parser pre_parser(pre_lexer, config);
        pre_parser.parseConfiguration();

        // 2. Main parse with loaded configuration
        Lexer lexer(source, config);
        Parser parser(lexer, config);
        std::unique_ptr<ElementNode> ast = parser.parse();

        if (ast) {
            Generator generator(parser, config);
            std::string html = generator.generate(*ast);
            std::cout << html << std::endl;
        } else {
            std::cerr << "Error: Parsing failed." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
