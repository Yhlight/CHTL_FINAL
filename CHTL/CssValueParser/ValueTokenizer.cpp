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
            case '?': tokens.push_back(makeToken(ValueTokenType::TOKEN_QUESTION, "?")); break;
            case ':': tokens.push_back(makeToken(ValueTokenType::TOKEN_COLON, ":")); break;
            case '.': tokens.push_back(makeToken(ValueTokenType::TOKEN_DOT, ".")); break;
            case '*':
                if (peek() == '*') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_POWER, "**")); }
                else { tokens.push_back(makeToken(ValueTokenType::TOKEN_STAR, "*")); }
                break;
            case '>':
                if (peek() == '=') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_GREATER_EQUAL, ">=")); }
                else { tokens.push_back(makeToken(ValueTokenType::TOKEN_GREATER, ">")); }
                break;
            case '<':
                if (peek() == '=') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_LESS_EQUAL, "<=")); }
                else { tokens.push_back(makeToken(ValueTokenType::TOKEN_LESS, "<")); }
                break;
            case '=':
                if (peek() == '=') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_EQUAL_EQUAL, "==")); }
                break;
            case '!':
                if (peek() == '=') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_NOT_EQUAL, "!=")); }
                break;
            case '&':
                if (peek() == '&') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_LOGICAL_AND, "&&")); }
                break;
            case '|':
                if (peek() == '|') { advance(); tokens.push_back(makeToken(ValueTokenType::TOKEN_LOGICAL_OR, "||")); }
                break;
            case '\'':
                tokens.push_back(stringLiteral());
                break;
            default:
                if (isdigit(c) || (c == '.' && isdigit(peek()))) {
                    current--;
                    tokens.push_back(number());
                } else if (isalpha(c) || c == '#' || c == '.') {
                    current--;
                    tokens.push_back(identifier());
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

ValueToken ValueTokenizer::number() {
    start = current;
    while (isdigit(peek()) || peek() == '.') {
        advance();
    }
    while (isalpha(peek()) || peek() == '%') {
        advance();
    }
    return makeToken(ValueTokenType::TOKEN_NUMBER, source.substr(start, current - start));
}

ValueToken ValueTokenizer::identifier() {
    start = current;
    while (isalnum(peek()) || peek() == '_' || peek() == '-' || peek() == '#') {
        advance();
    }
    return makeToken(ValueTokenType::TOKEN_IDENTIFIER, source.substr(start, current - start));
}

ValueToken ValueTokenizer::stringLiteral() {
    start = current; // Start after the opening '
    while (peek() != '\'' && !isAtEnd()) {
        advance();
    }

    if (isAtEnd()) return makeToken(ValueTokenType::TOKEN_ERROR, "Unterminated string.");

    std::string value = source.substr(start, current - start);
    advance(); // Consume the closing '

    return makeToken(ValueTokenType::TOKEN_STRING, value);
}
