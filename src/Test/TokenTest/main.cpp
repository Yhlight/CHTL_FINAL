#include "../../CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <vector>

// Helper function to convert TokenType to string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::L_BRACE: return "L_BRACE";
        case TokenType::R_BRACE: return "R_BRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        default: return "UNKNOWN";
    }
}

int main() {
    std::string chtl_source = R"(
// This is a test for the CHTL Lexer
html {
    head {
        title { "My Website" }
    }
    body {
        div {
            id: my-div;
            class = "container";
            text {
                Hello CHTL! // An unquoted literal
            }
        }
        -- A generator comment
    }
}
)";

    Lexer lexer(chtl_source);
    std::vector<Token> tokens;
    Token token = lexer.getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    tokens.push_back(token); // Add the EOF token

    for (const auto& t : tokens) {
        std::cout << "Line: " << t.line << ", Col: " << t.column
                  << "  Type: " << tokenTypeToString(t.type)
                  << "  Value: '" << t.value << "'" << std::endl;
    }

    return 0;
}
