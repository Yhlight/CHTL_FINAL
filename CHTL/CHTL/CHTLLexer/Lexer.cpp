#include "Lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        tokens.push_back(scanToken());
    }
    tokens.push_back(makeToken(TokenType::END_OF_FILE));
    return tokens;
}

char Lexer::advance() {
    if (!isAtEnd()) {
        current++;
        column++;
    }
    return source[current - 1];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, line, column - value.length()};
}

Token Lexer::makeToken(TokenType type, char value) {
    return Token{type, std::string(1, value), line, column - 1};
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (std::isspace(c)) {
            if (c == '\n') {
                line++;
                column = 0;
            }
            advance();
        } else if (c == '/' && peekNext() == '/') {
            while (peek() != '\n' && !isAtEnd()) advance();
        } else if (c == '/' && peekNext() == '*') {
            advance(); advance();
            while (!isAtEnd() && (peek() != '*' || peekNext() != '/')) {
                if (peek() == '\n') { line++; column = 0; }
                advance();
            }
            if (!isAtEnd()) { advance(); advance(); }
        } else {
            break;
        }
    }
}

Token Lexer::scanToken() {
    char c = peek();
    if (std::isalpha(c) || c == '_') return scanIdentifierOrKeyword();
    if (std::isdigit(c) || (c == '.' && std::isdigit(peekNext()))) return scanNumberLiteral();
    if (c == '"' || c == '\'') return scanStringLiteral();
    return scanSymbol();
}

Token Lexer::scanIdentifierOrKeyword() {
    size_t start = current;
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string value = source.substr(start, current - start);
    if (value == "text") return makeToken(TokenType::KEYWORD_TEXT, value);
    if (value == "style") return makeToken(TokenType::KEYWORD_STYLE, value);
    return makeToken(TokenType::IDENTIFIER, value);
}

Token Lexer::scanStringLiteral() {
    char quote_type = advance();
    size_t start = current;
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') { line++; column = 0; }
        advance();
    }
    if (isAtEnd()) return makeToken(TokenType::UNKNOWN, "Unterminated string");
    std::string value = source.substr(start, current - start);
    advance();
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::scanNumberLiteral() {
    size_t start = current;
    while (std::isdigit(peek())) advance();
    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) advance();
    }
    while (std::isalpha(peek()) || peek() == '%') advance();
    std::string value = source.substr(start, current - start);
    return makeToken(TokenType::NUMBER_LITERAL, value);
}

Token Lexer::scanSymbol() {
    size_t start = current;
    char c = advance();
    switch (c) {
        case '{': return makeToken(TokenType::LBRACE, c);
        case '}': return makeToken(TokenType::RBRACE, c);
        case ':': return makeToken(TokenType::COLON, c);
        case '=': return makeToken(TokenType::EQUALS, c);
        case ';': return makeToken(TokenType::SEMICOLON, c);
        case '[': return makeToken(TokenType::L_BRACKET, c);
        case ']': return makeToken(TokenType::R_BRACKET, c);
        case '(': return makeToken(TokenType::L_PAREN, c);
        case ')': return makeToken(TokenType::R_PAREN, c);
        case ',': return makeToken(TokenType::COMMA, c);
        case '@': return makeToken(TokenType::AT_SIGN, c);
        case '.': return makeToken(TokenType::DOT, c);
        case '&': return makeToken(TokenType::AMPERSAND, c);
        case '+': return makeToken(TokenType::PLUS, c);
        case '-': return makeToken(TokenType::MINUS, c);
        case '*': return makeToken(TokenType::STAR, c);
        case '/': return makeToken(TokenType::SLASH, c);
        case '#':
            // If we see a #, greedily consume hex characters for colors
            while (std::isxdigit(peek())) {
                advance();
            }
            return makeToken(TokenType::IDENTIFIER, source.substr(start, current - start));
        default:
            return makeToken(TokenType::UNKNOWN, std::string(1, c));
    }
}
