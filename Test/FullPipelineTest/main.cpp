#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>

int main() {
    std::string source = R"(
        div {
            id: container;

            style {
                width: 100px;
                background-color: #f0f0f0;
            }

            h1 {
                text { "Styled CHTL!" }
                style {
                    color: blue;
                }
            }
        }
    )";

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    try {
        // 1. Lexing
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 2. Parsing
        CHTL::CHTLParser parser(tokens);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // 3. Generation
        CHTL::CHTLGenerator generator;
        std::string html_output = generator.generate(ast.get());

        // 4. Verification
        std::cout << "--- Generated HTML ---\n" << html_output << "\n----------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
