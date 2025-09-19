#include "CHTLLexer.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

namespace CHTL {

static std::unordered_map<std::string, TokenType> keywords = {
    {"style", TokenType::STYLE},
    {"text", TokenType::TEXT},
    {"inherit", TokenType::INHERIT},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"except", TokenType::EXCEPT}
};

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, start});
    return tokens;
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case '@': addToken(TokenType::AT); break;
        case '.': addToken(TokenType::DOT); break;
        case ',': addToken(TokenType::COMMA); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '*': addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
        case ' ': case '\r': case '\t': break;
        case '\n': line++; break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) { advance(); advance(); } // Consume the */
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '"': case '\'': string(c); break;
        default:
            if (isdigit(c)) { number(); }
            else if (isalpha(c) || c == '_') { identifier(); }
            else { addToken(TokenType::SYMBOL); }
            break;
    }
}

char CHTLLexer::advance() { return source[current++]; }
void CHTLLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, start});
}
void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line, start});
}
bool CHTLLexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}
char CHTLLexer::peek() { return isAtEnd() ? '\0' : source[current]; }
char CHTLLexer::peekNext() { return current + 1 >= source.length() ? '\0' : source[current + 1]; }

void CHTLLexer::string(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) { return; } // Unterminated string
    advance(); // The closing quote.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLLexer::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void CHTLLexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    addToken(it != keywords.end() ? it->second : TokenType::IDENTIFIER);
}

} // namespace CHTL
