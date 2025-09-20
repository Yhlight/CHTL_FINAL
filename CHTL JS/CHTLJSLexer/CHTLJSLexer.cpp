#include "CHTLJSLexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL_JS {

static std::unordered_map<std::string, TokenType> keywords = {
    {"Listen", TokenType::LISTEN},
    {"Delegate", TokenType::DELEGATE},
    {"Animate", TokenType::ANIMATE},
};

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source) {}

std::vector<Token> CHTLJSLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, (int)source.length()});
    return tokens;
}

bool CHTLJSLexer::isAtEnd() {
    return current >= source.length();
}

void CHTLJSLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{':
            if (match('{')) {
                addToken(TokenType::LEFT_BRACE_BRACE);
            } else {
                addToken(TokenType::LEFT_BRACE);
            }
            break;
        case '}':
            if (match('}')) {
                addToken(TokenType::RIGHT_BRACE_BRACE);
            } else {
                addToken(TokenType::RIGHT_BRACE);
            }
            break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::SYMBOL); break;
        case '#': addToken(TokenType::SYMBOL); break;
        case '-':
            if (match('>')) {
                addToken(TokenType::ARROW);
            } else {
                addToken(TokenType::MINUS);
            }
            break;
        case '&':
            if (match('-') && match('>')) {
                addToken(TokenType::AMPERSAND_ARROW);
            }
            break;
        case ' ': case '\r': case '\t': break;
        case '\n': line++; break;
        case '"': case '\'': string(c); break;
        case '_':
            identifier(); // Handles placeholders and other identifiers starting with _
            break;
        default:
            if (isdigit(c)) { number(); }
            else if (isalpha(c)) { identifier(); }
            else { /* Ignore other characters for now */ }
            break;
    }
}

char CHTLJSLexer::advance() { return source[current++]; }
void CHTLJSLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, (int)start});
}
void CHTLJSLexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line, (int)start});
}
bool CHTLJSLexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}
char CHTLJSLexer::peek() { return isAtEnd() ? '\0' : source[current]; }
char CHTLJSLexer::peekNext() { return current + 1 >= source.length() ? '\0' : source[current + 1]; }

void CHTLJSLexer::string(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) { return; } // Unterminated string
    advance(); // The closing quote.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLJSLexer::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void CHTLJSLexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    addToken(it != keywords.end() ? it->second : TokenType::IDENTIFIER);
}

} // namespace CHTL_JS
