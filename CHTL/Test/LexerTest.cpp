#include "../CHTL/CHTLLexer/Lexer.h"
#include <iostream>

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ", Value: '" << token.value
                  << "', Line: " << token.line
                  << ", Col: " << token.column << std::endl;
    }
}

int main() {
    std::string source = R"(
// This is a CHTL example
html {
    head {
        title { text: "My CHTL Page"; }
    }
    body {
        div {
            id = main-content;
            class: "container";

            // A text block
            text {
                "Hello, CHTL!"
            }
        }
    }
}
)";

    std::cout << "--- Tokenizing Source ---" << std::endl;
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    printTokens(tokens);
    std::cout << "--- Tokenization Complete ---" << std::endl;

    return 0;
}
