#include "CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <vector>
#include <map>

// Helper to print token types as strings
std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeMap = {
        {TokenType::TOKEN_EOF, "EOF"},
        {TokenType::TOKEN_UNKNOWN, "UNKNOWN"},
        {TokenType::TOKEN_IDENTIFIER, "IDENTIFIER"},
        {TokenType::TOKEN_STRING, "STRING"},
        {TokenType::TOKEN_LBRACE, "LBRACE"},
        {TokenType::TOKEN_RBRACE, "RBRACE"},
        {TokenType::TOKEN_COLON, "COLON"},
        {TokenType::TOKEN_EQUAL, "EQUAL"},
        {TokenType::TOKEN_SEMICOLON, "SEMICOLON"},
        {TokenType::TOKEN_LPAREN, "LPAREN"},
        {TokenType::TOKEN_RPAREN, "RPAREN"},
        {TokenType::TOKEN_DOT, "DOT"},
        {TokenType::TOKEN_HASH, "HASH"},
        {TokenType::TOKEN_AMPERSAND, "AMPERSAND"},
        {TokenType::TOKEN_PLUS, "PLUS"},
        {TokenType::TOKEN_MINUS, "MINUS"},
        {TokenType::TOKEN_STAR, "STAR"},
        {TokenType::TOKEN_SLASH, "SLASH"},
        {TokenType::TOKEN_PERCENT, "PERCENT"},
        {TokenType::TOKEN_STAR_STAR, "STAR_STAR"},
        {TokenType::TOKEN_GREATER, "GREATER"},
        {TokenType::TOKEN_LESS, "LESS"},
        {TokenType::TOKEN_GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::TOKEN_LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::TOKEN_EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::TOKEN_BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::TOKEN_AMPERSAND_AMPERSAND, "AMPERSAND_AMPERSAND"},
        {TokenType::TOKEN_PIPE_PIPE, "PIPE_PIPE"},
        {TokenType::TOKEN_QUESTION, "QUESTION"},
        {TokenType::TOKEN_NUMBER, "NUMBER"},
        {TokenType::TOKEN_LINE_COMMENT, "LINE_COMMENT"},
        {TokenType::TOKEN_BLOCK_COMMENT, "BLOCK_COMMENT"},
        {TokenType::TOKEN_GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        {TokenType::TOKEN_KEYWORD_TEXT, "KEYWORD_TEXT"},
        {TokenType::TOKEN_KEYWORD_STYLE, "KEYWORD_STYLE"}
    };
    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }
    return "UNHANDLED_TOKEN_TYPE";
}

int main() {
    std::string source = R"(
        // A simple CHTL example
        html {
            body {
                div {
                    id: "main-content";
                    class = "container"; -- a generator comment

                    /*
                        Multi-line comment
                    */
                    text {
                        'Hello, CHTL!'
                    }

                    style {
                        .new-class { color: blue; }
                        #new-id { font-size: 16px; }
                        &:hover { background: white; }
                        width: 100 + 50 * 2;
                        height: (200 / 2) % 3 ** 4;
                        color: width > 150 ? "red" : "blue";
                    }
                }
            }
        }
    )";

    std::cout << "--- Starting Lexer Test ---" << std::endl;
    std::cout << "Source code:\n" << source << std::endl;
    std::cout << "--- Tokens ---" << std::endl;

    Lexer lexer(source);
    Token token;
    int tokenCount = 0;
    do {
        token = lexer.nextToken();
        std::cout << "Token " << ++tokenCount
                  << " | Type: " << tokenTypeToString(token.type)
                  << " | Lexeme: '" << token.lexeme << "'"
                  << " | Line: " << token.line << std::endl;
    } while (token.type != TokenType::TOKEN_EOF);

    std::cout << "--- Lexer Test Finished ---" << std::endl;

    return 0;
}
