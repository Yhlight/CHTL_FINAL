#include "../CHTL/Lexer/Lexer.h"
#include "../CHTL/Parser/Parser.h"
#include "../CHTL/Generator/Generator.h"
#include "AstPrinter.h"
#include <iostream>
#include <vector>

int main() {
    std::string source = R"(
        html {
            body {
                div {
                    id: "main-box";
                    style {
                        color: red;
                        font-size: 16px;
                    }
                    text {
                        "Hello, CHTL!"
                    }
                }
            }
        }
    )";

    // 1. Lexing
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    // 2. Parsing
    CHTL::Parser parser(tokens);
    try {
        std::shared_ptr<CHTL::RootNode> ast = parser.parse();

        // Optional: Print AST for debugging
        CHTL::AstPrinter printer;
        std::cout << "--- Abstract Syntax Tree ---\n";
        std::cout << printer.print(ast) << std::endl;

        // 3. Generating
        CHTL::Generator generator(ast);
        generator.generate();

        std::cout << "\n--- Generated HTML ---\n";
        std::cout << generator.getHtml() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
