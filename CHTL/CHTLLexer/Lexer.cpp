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
    return Token{TokenType::END_OF_FILE, message, line, column};
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
                    advance();
                    advance();
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) {
                        advance();
                        advance();
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
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string text = source.substr(start, current - start);
    if (text == "text") return makeToken(TokenType::TEXT, text);
    if (text == "style") return makeToken(TokenType::STYLE, text);
    if (text == "inherit") return makeToken(TokenType::KEYWORD_INHERIT, text);
    if (text == "delete") return makeToken(TokenType::KEYWORD_DELETE, text);
    return makeToken(TokenType::IDENTIFIER, text);
}

Token Lexer::stringLiteral() {
    char quote = peek();
    advance();
    size_t start = current;
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string.");
    std::string value = source.substr(start, current - start);
    advance();
    return makeToken(TokenType::STRING, value);
}

Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE, "");
    }

    if (peek() == '[') {
        if (source.substr(current, 10) == "[Template]") {
            current += 10;
            return makeToken(TokenType::KEYWORD_TEMPLATE, "[Template]");
        }
        if (source.substr(current, 8) == "[Custom]") {
            current += 8;
            return makeToken(TokenType::KEYWORD_CUSTOM, "[Custom]");
        }
    }

    if (peek() == '@') {
        if (source.substr(current, 6) == "@Style") {
            current += 6;
            return makeToken(TokenType::AT_STYLE, "@Style");
        }
        if (source.substr(current, 8) == "@Element") {
            current += 8;
            return makeToken(TokenType::AT_ELEMENT, "@Element");
        }
        if (source.substr(current, 4) == "@Var") {
            current += 4;
            return makeToken(TokenType::AT_VAR, "@Var");
        }
    }

    char c = peek();
    if (isalpha(c) || c == '_') return identifier();
    if (c == '"' || c == '\'') return stringLiteral();

    switch (c) {
        case '(': advance(); return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': advance(); return makeToken(TokenType::RIGHT_PAREN, ")");
        case '{': advance(); return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': advance(); return makeToken(TokenType::RIGHT_BRACE, "}");
        case ':': advance(); return makeToken(TokenType::COLON, ":");
        case '=': advance(); return makeToken(TokenType::EQUAL, "=");
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";");
    }

    // Fallback for unquoted literals like '16px'
    size_t start = current;
    while (!isspace(peek()) && peek() != ';' && peek() != '}' && peek() != '{' && !isAtEnd()) {
        advance();
    }
    if (current > start) {
        std::string text = source.substr(start, current - start);
        return makeToken(TokenType::STRING, text);
    }

    advance();
    return errorToken("Unexpected character.");
}
