#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "ASTPrinter.h"
#include <iostream>

int main() {
    std::string source = R"(
        html {
            id: main_page;

            body {
                // A title
                h1 {
                    text { "Welcome to CHTL" }
                }
            }
        }
    )";

    try {
        // 1. Lexing
        std::cout << "--- Lexing ---" << std::endl;
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        std::cout << "Lexing complete. " << tokens.size() << " tokens found." << std::endl;

        // 2. Parsing
        std::cout << "\n--- Parsing ---" << std::endl;
        CHTL::CHTLParser parser(source, tokens, "test.chtl");
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        std::cout << "Parsing complete." << std::endl;

        // 3. Printing the AST for verification
        std::cout << "\n--- AST Verification ---" << std::endl;
        CHTL::ASTPrinter printer;
        printer.print(ast.get());
        std::cout << "------------------------" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
