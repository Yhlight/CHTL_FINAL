#include "CHTLJSLexer.h"
#include <cctype>

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source), start(0), current(0), line(1) {}

bool CHTLJSLexer::isAtEnd() { return current >= source.length(); }
char CHTLJSLexer::advance() { current++; return source[current - 1]; }
char CHTLJSLexer::peek() { if (isAtEnd()) return '\0'; return source[current]; }
bool CHTLJSLexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type) {
    return CHTLJSToken{type, source.substr(start, current - start), line};
}

CHTLJSToken CHTLJSLexer::errorToken(const std::string& message) {
    return CHTLJSToken{CHTLJSTokenType::TOKEN_UNKNOWN, message, line};
}

void CHTLJSLexer::skipWhitespace() {
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
            default:
                return;
        }
    }
}

CHTLJSToken CHTLJSLexer::identifierToken() {
    while (isalnum(peek()) || peek() == '_' || peek() == '.' || peek() == '#' || peek() == '-') advance();
    return makeToken(CHTLJSTokenType::TOKEN_IDENTIFIER);
}

CHTLJSToken CHTLJSLexer::stringToken() {
    char quote = source[start];
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string.");
    advance();
    return makeToken(CHTLJSTokenType::TOKEN_STRING);
}

CHTLJSToken CHTLJSLexer::numberToken() {
     while (isdigit(peek())) advance();
    if (peek() == '.' && current + 1 < source.length() && isdigit(source[current+1])) {
        advance();
        while (isdigit(peek())) advance();
    }
    return makeToken(CHTLJSTokenType::TOKEN_NUMBER);
}


CHTLJSToken CHTLJSLexer::nextToken() {
    skipWhitespace();
    start = current;
    if (isAtEnd()) return makeToken(CHTLJSTokenType::TOKEN_EOF);

    char c = advance();

    if (isalpha(c) || c == '.' || c == '#') return identifierToken();
    if (isdigit(c)) return numberToken();

    switch (c) {
        case '{':
            if (match('{')) return makeToken(CHTLJSTokenType::TOKEN_DOUBLE_LBRACE);
            return makeToken(CHTLJSTokenType::TOKEN_LBRACE);
        case '}':
            if (match('}')) return makeToken(CHTLJSTokenType::TOKEN_DOUBLE_RBRACE);
            return makeToken(CHTLJSTokenType::TOKEN_RBRACE);
        case '-':
            if (match('>')) return makeToken(CHTLJSTokenType::TOKEN_ARROW);
            break; // Fallthrough
        case ':': return makeToken(CHTLJSTokenType::TOKEN_COLON);
        case ',': return makeToken(CHTLJSTokenType::TOKEN_COMMA);
        case '(': return makeToken(CHTLJSTokenType::TOKEN_LPAREN);
        case ')': return makeToken(CHTLJSTokenType::TOKEN_RPAREN);
        case '[': return makeToken(CHTLJSTokenType::TOKEN_LBRACKET);
        case ']': return makeToken(CHTLJSTokenType::TOKEN_RBRACKET);
        case '"': case '\'': return stringToken();
    }

    return errorToken("Unexpected character.");
}
