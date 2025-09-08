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
    return Token{type, lexeme, line, column - (int)lexeme.length(), current - lexeme.length()};
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

Token Lexer::errorToken(const std::string& message) {
    return Token{TokenType::END_OF_FILE, message, line, column, current};
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
    if (text == "from") return makeToken(TokenType::KEYWORD_FROM, text);
    if (text == "as") return makeToken(TokenType::KEYWORD_AS, text);
    if (text == "inherit") return makeToken(TokenType::KEYWORD_INHERIT, text);
    if (text == "delete") return makeToken(TokenType::KEYWORD_DELETE, text);
    if (text == "insert") return makeToken(TokenType::KEYWORD_INSERT, text);
    if (text == "after") return makeToken(TokenType::KEYWORD_AFTER, text);
    if (text == "before") return makeToken(TokenType::KEYWORD_BEFORE, text);
    if (text == "replace") return makeToken(TokenType::KEYWORD_REPLACE, text);
    if (text == "at") { // For "at top" and "at bottom"
        size_t nextPos = current;
        while (isspace(source[nextPos])) nextPos++;
        if (source.substr(nextPos, 3) == "top") {
            current = nextPos + 3;
            return makeToken(TokenType::KEYWORD_ATTOP, "at top");
        }
        if (source.substr(nextPos, 6) == "bottom") {
            current = nextPos + 6;
            return makeToken(TokenType::KEYWORD_ATBOTTOM, "at bottom");
        }
    }
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

Token Lexer::number() {
    size_t start = current;
    while (isdigit(peek())) {
        advance();
    }
    std::string text = source.substr(start, current - start);
    return makeToken(TokenType::NUMBER, text);
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
        if (source.substr(current, 8) == "[Origin]") {
            current += 8;
            return makeToken(TokenType::KEYWORD_ORIGIN, "[Origin]");
        }
        if (source.substr(current, 8) == "[Import]") {
            current += 8;
            return makeToken(TokenType::KEYWORD_IMPORT, "[Import]");
        }
    }

    if (peek() == '@') {
        advance();
        return makeToken(TokenType::AT, "@");
    }

    char c = peek();
    if (isdigit(c)) return number();
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
        case '[': advance(); return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': advance(); return makeToken(TokenType::RIGHT_BRACKET, "]");
        case ',': advance(); return makeToken(TokenType::COMMA, ",");
        case '.': advance(); return makeToken(TokenType::DOT, ".");
        case '#': advance(); return makeToken(TokenType::HASH, "#");
        case '&':
            advance();
            if (match('&')) return makeToken(TokenType::AND_AND, "&&");
            return makeToken(TokenType::AMPERSAND, "&");
        case '|':
            advance();
            if (match('|')) return makeToken(TokenType::OR_OR, "||");
            return makeToken(TokenType::IDENTIFIER, "|"); // Or some other default
        case '>':
            advance();
            if (match('=')) return makeToken(TokenType::GREATER_EQUAL, ">=");
            return makeToken(TokenType::GREATER, ">");
        case '<':
            advance();
            if (match('=')) return makeToken(TokenType::LESS_EQUAL, "<=");
            return makeToken(TokenType::LESS, "<");
        case '!':
            advance();
            if (match('=')) return makeToken(TokenType::NOT_EQUAL, "!=");
            return makeToken(TokenType::IDENTIFIER, "!"); // Or some other default
        case '?': advance(); return makeToken(TokenType::QUESTION, "?");
        case '/': advance(); return makeToken(TokenType::SLASH, "/");
        case '*': advance(); return makeToken(TokenType::STAR, "*");
        case '+': advance(); return makeToken(TokenType::PLUS, "+");
        case '-': advance(); return makeToken(TokenType::MINUS, "-");
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
