#include "Lexer.h"

const std::map<std::string, CHTLJSTokenType> CHTLJSLexer::keywords = {
    {"fileloader", CHTLJSTokenType::FileLoader},
    {"listen", CHTLJSTokenType::Listen},
    {"delegate", CHTLJSTokenType::Delegate},
    {"animate", CHTLJSTokenType::Animate},
    {"vir", CHTLJSTokenType::Vir},
    {"router", CHTLJSTokenType::Router},
    {"util", CHTLJSTokenType::Util},
    {"then", CHTLJSTokenType::Then},
    {"iNeverAway", CHTLJSTokenType::INeverAway},
    {"printMylove", CHTLJSTokenType::PrintMyLove}
};

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source) {}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    CHTLJSToken token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != CHTLJSTokenType::EndOfFile);
    return tokens;
}

CHTLJSToken CHTLJSLexer::nextToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(CHTLJSTokenType::EndOfFile);

    char c = advance();

    switch (c) {
        case '{':
            if (peek() == '{') { advance(); return makeToken(CHTLJSTokenType::DoubleCurlyOpen); }
            return makeToken(CHTLJSTokenType::OpenBrace);
        case '}':
            if (peek() == '}') { advance(); return makeToken(CHTLJSTokenType::DoubleCurlyClose); }
            return makeToken(CHTLJSTokenType::CloseBrace);
        case '(': return makeToken(CHTLJSTokenType::OpenParen);
        case ')': return makeToken(CHTLJSTokenType::CloseParen);
        case '[': return makeToken(CHTLJSTokenType::OpenBracket);
        case ']': return makeToken(CHTLJSTokenType::CloseBracket);
        case ':': return makeToken(CHTLJSTokenType::Colon);
        case ';': return makeToken(CHTLJSTokenType::Semicolon);
        case ',': return makeToken(CHTLJSTokenType::Comma);
        // case '.': return makeToken(CHTLJSTokenType::Dot); // Dot is part of identifiers now
        case '=':
            if (peek() == '>') { advance(); return makeToken(CHTLJSTokenType::FatArrow); }
            return makeToken(CHTLJSTokenType::Equals);
        case '-':
            if (peek() == '>') { advance(); return makeToken(CHTLJSTokenType::Arrow); }
            break; // Fall through to handle as unexpected
        case '"':
        case '\'':
            return stringLiteral(c);
        default:
            if (isAlpha(c)) return identifier();
            if (isDigit(c)) return number();
            // Check for identifiers that don't start with alpha, like ./path
            if (isIdentifierChar(c)) return identifier();
            break;
    }

    return errorToken("Unexpected character.");
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
                column = 1;
                advance();
                break;
            default:
                return;
        }
    }
}

CHTLJSToken CHTLJSLexer::identifier() {
    while (isIdentifierChar(peek())) {
        advance();
    }
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second);
    }
    return makeToken(CHTLJSTokenType::Identifier);
}

CHTLJSToken CHTLJSLexer::number() {
    while (isDigit(peek())) {
        advance();
    }
    // Handle floating point
    if (peek() == '.' && isDigit(source[current + 1])) {
        advance(); // consume '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    return makeToken(CHTLJSTokenType::NumberLiteral);
}

CHTLJSToken CHTLJSLexer::stringLiteral(char opening_quote) {
    while (peek() != opening_quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string.");
    advance(); // Closing quote
    return makeToken(CHTLJSTokenType::StringLiteral);
}


CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type) const {
    return makeToken(type, source.substr(start, current - start));
}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type, const std::string& value) const {
    return CHTLJSToken{type, value, line, (int)(column - (current - start))};
}

CHTLJSToken CHTLJSLexer::errorToken(const std::string& message) const {
    return CHTLJSToken{CHTLJSTokenType::Unexpected, message, line, column};
}

char CHTLJSLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLJSLexer::advance() {
    if (!isAtEnd()) {
        current++;
        column++;
    }
    return source[current - 1];
}

bool CHTLJSLexer::isAtEnd() const {
    return current >= source.length();
}

bool CHTLJSLexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool CHTLJSLexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool CHTLJSLexer::isIdentifierChar(char c) const {
    switch (c) {
        case '{':
        case '}':
        case '(':
        case ')':
        case '[':
        case ']':
        case ':':
        case ';':
        case ',':
        case '=':
        case '\0':
            return false;
        default:
            return !isspace(c);
    }
}
