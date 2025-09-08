#include "Lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token{type, lexeme, line, column - (int)lexeme.length()};
}

Token Lexer::errorToken(const std::string& message) {
    return Token{TokenType::END_OF_FILE, message, line, column}; // Using EOF as an error indicator for now
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    advance(); // Consume '/'
                    advance(); // Consume '*'
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) {
                        advance(); // Consume '*'
                        advance(); // Consume '/'
                    }
                } else {
                    return;
                }
                break;
            case '-':
                 if (peekNext() == '-') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::identifier() {
    size_t start = current;
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string text = source.substr(start, current - start);
    if (text == "text") return makeToken(TokenType::TEXT, text);
    return makeToken(TokenType::IDENTIFIER, text);
}

Token Lexer::stringLiteral() {
    char quote = advance(); // Consume the opening quote
    size_t start = current;
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    advance(); // Consume the closing quote.

    std::string value = source.substr(start, current - start -1);
    return makeToken(TokenType::STRING, value);
}


Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE, "");
    }

    char c = advance();

    if (isalpha(c) || c == '_') {
        // Backtrack one character as identifier() expects to start at the beginning of the identifier.
        current--;
        column--;
        return identifier();
    }

    if (c == '"' || c == '\'') {
         // Backtrack one character as stringLiteral() expects to start at the opening quote.
        current--;
        column--;
        return stringLiteral();
    }

    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case ':': return makeToken(TokenType::COLON, ":");
        case '=': return makeToken(TokenType::EQUAL, "=");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
    }

    // Unquoted literals for attributes and text
    if (isalnum(c) || c == '#') { // Simple check for things that can start an unquoted literal
        size_t start = current - 1;
         while (!isspace(peek()) && peek() != ';' && peek() != '}' && !isAtEnd()) {
            advance();
        }
        std::string text = source.substr(start, current - start);
        return makeToken(TokenType::STRING, text);
    }


    return errorToken("Unexpected character.");
}
