#include "../CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <map>

// Helper to print token types as strings
std::map<TokenType, std::string> tokenTypeNames = {
    {TokenType::TOKEN_EOF, "EOF"},
    {TokenType::TOKEN_ERROR, "ERROR"},
    {TokenType::TOKEN_IDENTIFIER, "IDENTIFIER"},
    {TokenType::TOKEN_STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::TOKEN_UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::TOKEN_LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::TOKEN_RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::TOKEN_COLON, "COLON"},
    {TokenType::TOKEN_EQUALS, "EQUALS"},
    {TokenType::TOKEN_SEMICOLON, "SEMICOLON"},
    {TokenType::TOKEN_LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::TOKEN_RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::TOKEN_AT, "AT"},
    {TokenType::TOKEN_DOLLAR, "DOLLAR"},
    {TokenType::TOKEN_AMPERSAND, "AMPERSAND"},
    {TokenType::TOKEN_PLUS, "PLUS"},
    {TokenType::TOKEN_MINUS, "MINUS"},
    {TokenType::TOKEN_STAR, "STAR"},
    {TokenType::TOKEN_SLASH, "SLASH"},
    {TokenType::TOKEN_PERCENT, "PERCENT"},
    {TokenType::TOKEN_COMMENT_NORMAL, "COMMENT_NORMAL"},
    {TokenType::TOKEN_COMMENT_GENERATOR, "COMMENT_GENERATOR"},
};


void printToken(const Token& token) {
    std::string typeName = tokenTypeNames.count(token.type) ? tokenTypeNames[token.type] : "UNKNOWN";
    std::cout << "Line " << token.line << "\t"
              << typeName << "\t\t"
              << "'" << token.value << "'" << std::endl;
}

int main() {
    const char* sample_code = R"CHTL(
// This is a normal comment
# This is a generator comment

html {
    body {
        div {
            id: "main-content";
            class = box; // class with unquoted literal

            /*
             * Multi-line comment
             */
            text {
                'Hello, CHTL!'
            }
        }
    }
}
)CHTL";

    std::cout << "--- Tokenizing Sample CHTL Code ---" << std::endl;
    Lexer lexer(sample_code);

    Token token;
    do {
        token = lexer.nextToken();
        printToken(token);
    } while (token.type != TokenType::TOKEN_EOF);

    std::cout << "--- End of Tokenization ---" << std::endl;

    return 0;
}
