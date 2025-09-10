#include <iostream>
#include <vector>
#include <memory>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"

int main() {
    std::string source = R"(
        [Template] @Var Theme {
            primaryColor: blue;
            baseMargin: 10px;
        }

        div {
            style {
                color: Theme(primaryColor);
                margin: Theme(baseMargin) * 2;
            }
        }
    )";

    std::cout << "--- CHTL Compilation (Variable Template Test) ---" << std::endl;

    try {
        // 1. Lexing
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.Tokenize();

        // 2. Parsing
        CHTL::CHTLParser parser(tokens);
        CHTL::NodeList ast = parser.Parse();

        if (ast.empty()) {
            std::cerr << "Parsing failed. Aborting." << std::endl;
            return 1;
        }

        // 3. Generation
        CHTL::CHTLGenerator generator;
        std::string html_output = generator.Generate(ast);

        std::cout << "Output:\n" << html_output << std::endl;
        std::cout << "Compilation successful." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
