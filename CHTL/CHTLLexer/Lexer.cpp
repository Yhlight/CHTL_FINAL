#include "Lexer.h"
#include <map>

// A map to check for keywords
static std::map<std::string, TokenType> keywords = {
    {"text", TokenType::TOKEN_TEXT},
    {"style", TokenType::TOKEN_STYLE},
    {"Style", TokenType::TOKEN_STYLE}, // For @Style
    {"Element", TokenType::TOKEN_ELEMENT} // For @Element
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        start = current;
        Token token = scanToken();
        tokens.push_back(token);
        if (token.type == TokenType::TOKEN_EOF) break;
    }
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

Token Lexer::scanToken() {
    skipWhitespaceAndComments();
    start = current;

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance();

    if (isalpha(c) || c == '_') return identifier();
    if (isdigit(c)) { /* For now, we don't handle numbers outside of strings */ }

    switch (c) {
        case '{': return makeToken(TokenType::TOKEN_LBRACE);
        case '}': return makeToken(TokenType::TOKEN_RBRACE);
        case '[': return makeToken(TokenType::TOKEN_LBRACKET);
        case ']': return makeToken(TokenType::TOKEN_RBRACKET);
        case '@': return makeToken(TokenType::TOKEN_AT);
        case ':': return makeToken(TokenType::TOKEN_COLON);
        case '=': return makeToken(TokenType::TOKEN_EQUAL);
        case ';': return makeToken(TokenType::TOKEN_SEMICOLON);
        case '"':
        case '\'': return stringLiteral();
        case '-':
            if (match('-')) return generatorComment();
            break;
    }

    return errorToken("Unexpected character.");
}

char Lexer::advance() {
    current++;
    return source[current - 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
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
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // A // comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    // A /* */ comment.
                    advance(); // consume /
                    advance(); // consume *
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (!isAtEnd()) {
                        advance(); // consume *
                        advance(); // consume /
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type) {
    return {type, source.substr(start, current - start), line, start};
}

Token Lexer::errorToken(const std::string& message) {
    return {TokenType::TOKEN_ERROR, message, line, start};
}

Token Lexer::stringLiteral() {
    char quote_type = source[start]; // " or '
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance(); // The closing quote.

    // Trim the surrounding quotes
    std::string value = source.substr(start + 1, current - start - 2);
    return {TokenType::TOKEN_STRING_LITERAL, value, line};
}

Token Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::TOKEN_IDENTIFIER;

    if (keywords.count(text)) {
        type = keywords[text];
    }

    return makeToken(type);
}

Token Lexer::generatorComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    // Don't include the -- in the lexeme
    return {TokenType::TOKEN_GENERATOR_COMMENT, source.substr(start + 2, current - start - 2), line};
}
