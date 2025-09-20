#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <vector>

int main() {
    std::string source = R"(
        // This is a test of the CHTL lexer
        div {
            id: 'main_box'; // an id
            /* This is a
               multi-line comment */
            text: "Hello World";
            width: 100;
        }
    )";

    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Type: " << CHTL::tokenTypeToString(token.type)
                  << ", Lexeme: '" << token.lexeme
                  << "', Line: " << token.line << std::endl;
    }

    return 0;
}
