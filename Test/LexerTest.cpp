#include "Lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

// A simple helper to convert TokenType to string for printing
std::string tokenTypeToString(CHTL::TokenType type) {
    switch (type) {
        case CHTL::TokenType::ILLEGAL: return "ILLEGAL";
        case CHTL::TokenType::END_OF_FILE: return "END_OF_FILE";
        case CHTL::TokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTL::TokenType::STRING: return "STRING";
        case CHTL::TokenType::LBRACE: return "LBRACE";
        case CHTL::TokenType::RBRACE: return "RBRACE";
        case CHTL::TokenType::COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

void runLexerTest() {
    std::cout << "Running Lexer Test..." << std::endl;

    std::string input = R"(
        // This is a comment
        html {
            body {
                text { "Hello World" }
            }
        }
        /* Another
           multiline comment */
    )";

    std::vector<std::pair<CHTL::TokenType, std::string>> expected_tokens = {
        {CHTL::TokenType::IDENTIFIER, "html"},
        {CHTL::TokenType::LBRACE, "{"},
        {CHTL::TokenType::IDENTIFIER, "body"},
        {CHTL::TokenType::LBRACE, "{"},
        {CHTL::TokenType::IDENTIFIER, "text"},
        {CHTL::TokenType::LBRACE, "{"},
        {CHTL::TokenType::STRING, "Hello World"},
        {CHTL::TokenType::RBRACE, "}"},
        {CHTL::TokenType::RBRACE, "}"},
        {CHTL::TokenType::RBRACE, "}"},
        {CHTL::TokenType::END_OF_FILE, ""}
    };

    CHTL::Lexer lexer(input);
    bool success = true;

    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        CHTL::Token token = lexer.NextToken();
        auto& expected = expected_tokens[i];

        if (token.type != expected.first || token.literal != expected.second) {
            std::cerr << "Test Failed!" << std::endl;
            std::cerr << "Token " << i << ": Expected [" << tokenTypeToString(expected.first)
                      << ", \"" << expected.second << "\"], but got ["
                      << tokenTypeToString(token.type) << ", \"" << token.literal << "\"]"
                      << std::endl;
            success = false;
            break;
        }
    }

    if (success) {
        std::cout << "Lexer Test Passed!" << std::endl;
    } else {
        // The assert will cause a non-zero exit code, which CTest interprets as failure.
        assert(false);
    }
}

int main() {
    runLexerTest();
    return 0;
}
