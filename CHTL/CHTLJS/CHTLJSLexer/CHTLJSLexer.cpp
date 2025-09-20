#include "CHTLJSLexer.h"
#include <map>

namespace CHTLJS {

const std::map<std::string, TokenType> CHTLJSLexer::keywords = {
    {"Listen", TokenType::LISTEN},
    {"Animate", TokenType::ANIMATE},
    {"Delegate", TokenType::DELEGATE},
    {"Vir", TokenType::VIR},
};

CHTLJSLexer::CHTLJSLexer(const std::string& source)
    : source(source) {}

std::vector<Token> CHTLJSLexer::scanTokens() {
    while (current < source.length()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

void CHTLJSLexer::scanToken() {
    char c = advance();
    switch (c) {
        case ':': addToken(TokenType::COLON); break;
        case '{':
            if (match('{')) {
                addToken(TokenType::LBRACE_BRACE);
            } else {
                addToken(TokenType::LEFT_BRACE);
            }
            break;
        case '}':
            if (match('}')) {
                addToken(TokenType::RBRACE_BRACE);
            } else {
                addToken(TokenType::RIGHT_BRACE);
            }
            break;
        case '-':
            if (match('>')) {
                addToken(TokenType::ARROW);
            } else {
                addToken(TokenType::MINUS);
            }
            break;
        case '"':
        case '\'':
            string();
            break;
        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;

        default:
            if (isalpha(c) || c == '_') {
                identifier();
            } else {
                // For now, treat other symbols as unknown, can be expanded
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}

char CHTLJSLexer::advance() {
    return source[current++];
}

void CHTLJSLexer::addToken(TokenType type) {
    addToken(type, "");
}

void CHTLJSLexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

bool CHTLJSLexer::match(char expected) {
    if (current >= source.length()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char CHTLJSLexer::peek() {
    if (current >= source.length()) return '\0';
    return source[current];
}

void CHTLJSLexer::string() {
    char quote_type = source[start];
    while (peek() != quote_type && current < source.length()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (current >= source.length()) {
        // Unterminated string, handle error appropriately
        return;
    }

    advance(); // Closing quote
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLJSLexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords.at(text);
    }
    addToken(type);
}

} // namespace CHTLJS
