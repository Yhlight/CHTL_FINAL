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
            // Single-line comment
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
        } else if (c == '/' && peekNext() == '*') {
            // Multi-line comment
            advance(); // consume /
            advance(); // consume *
            while (!isAtEnd() && (peek() != '*' || peekNext() != '/')) {
                if (peek() == '\n') {
                    line++;
                    column = 0;
                }
                advance();
            }
            if (!isAtEnd()) {
                advance(); // consume *
                advance(); // consume /
            }
        } else {
            break;
        }
    }
}

Token Lexer::scanToken() {
    char c = peek();

    if (std::isalpha(c) || c == '_') {
        return scanIdentifierOrKeyword();
    }
    if (std::isdigit(c)) {
        return scanNumberLiteral();
    }
    if (c == '"' || c == '\'') {
        return scanStringLiteral();
    }

    return scanSymbol();
}

Token Lexer::scanIdentifierOrKeyword() {
    size_t start = current;
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string value = source.substr(start, current - start);

    // Check for keywords
    if (value == "text") return makeToken(TokenType::KEYWORD_TEXT, value);
    if (value == "style") return makeToken(TokenType::KEYWORD_STYLE, value);

    // It's a regular identifier (or a tag name, handled by parser)
    // For now, let's use a generic IDENTIFIER type. The parser can decide if it's a tag or attribute.
    return makeToken(TokenType::IDENTIFIER, value);
}

Token Lexer::scanStringLiteral() {
    char quote_type = advance(); // Consume opening quote
    size_t start = current;
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        return makeToken(TokenType::UNKNOWN, "Unterminated string");
    }

    std::string value = source.substr(start, current - start);
    advance(); // Consume closing quote
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::scanNumberLiteral() {
    size_t start = current;
    while (std::isdigit(peek()) || (peek() == '.' && std::isdigit(peekNext()))) {
        advance();
    }
    std::string value = source.substr(start, current - start);
    return makeToken(TokenType::NUMBER_LITERAL, value);
}

Token Lexer::scanSymbol() {
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
        default:
            return makeToken(TokenType::UNKNOWN, std::string(1, c));
    }
}
