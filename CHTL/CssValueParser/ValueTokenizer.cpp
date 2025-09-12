#include "ValueTokenizer.h"

ValueTokenizer::ValueTokenizer(const std::string& source) : source(source) {}

std::vector<ValueToken> ValueTokenizer::tokenize() {
    std::vector<ValueToken> tokens;
    while (!isAtEnd()) {
        start = current;
        char c = advance();

        switch (c) {
            case ' ': case '\r': case '\t': case '\n': break; // Ignore whitespace
            case '(': tokens.push_back(makeToken(ValueTokenType::TOKEN_LPAREN, "(")); break;
            case ')': tokens.push_back(makeToken(ValueTokenType::TOKEN_RPAREN, ")")); break;
            case '+': tokens.push_back(makeToken(ValueTokenType::TOKEN_PLUS, "+")); break;
            case '-': tokens.push_back(makeToken(ValueTokenType::TOKEN_MINUS, "-")); break;
            case '/': tokens.push_back(makeToken(ValueTokenType::TOKEN_SLASH, "/")); break;
            case '*':
                if (peek() == '*') {
                    advance();
                    tokens.push_back(makeToken(ValueTokenType::TOKEN_POWER, "**"));
                } else {
                    tokens.push_back(makeToken(ValueTokenType::TOKEN_STAR, "*"));
                }
                break;
            default:
                if (isdigit(c) || c == '.') {
                    current--; // Go back to parse the full number and its unit
                    tokens.push_back(number());
                } else {
                    tokens.push_back(makeToken(ValueTokenType::TOKEN_ERROR, std::string(1, c)));
                }
                break;
        }
    }
    tokens.push_back(makeToken(ValueTokenType::TOKEN_EOF, ""));
    return tokens;
}

bool ValueTokenizer::isAtEnd() {
    return current >= source.length();
}

char ValueTokenizer::advance() {
    return source[current++];
}

char ValueTokenizer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

ValueToken ValueTokenizer::makeToken(ValueTokenType type, const std::string& text) {
    return {type, text};
}

// Parses a number and its optional immediately following unit (e.g., "100px", "5.5em", "20%")
ValueToken ValueTokenizer::number() {
    start = current;
    // Consume digits
    while (isdigit(peek()) || peek() == '.') {
        advance();
    }
    // Consume optional unit (letters or %)
    while (isalpha(peek()) || peek() == '%') {
        advance();
    }
    return makeToken(ValueTokenType::TOKEN_NUMBER, source.substr(start, current - start));
}

// unit() helper is no longer needed
// ValueToken ValueTokenizer::unit() { ... }
