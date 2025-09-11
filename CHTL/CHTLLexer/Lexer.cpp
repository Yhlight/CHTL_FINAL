#include "Lexer.h"
#include <iostream>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token{TokenType::END_OF_FILE, "", "", line});
    return tokens;
}

bool Lexer::isAtEnd() { return current >= source.length(); }
char Lexer::advance() { current++; return source[current - 1]; }
char Lexer::peek() { if (isAtEnd()) return '\0'; return source[current]; }
char Lexer::peekNext() { if (current + 1 >= source.length()) return '\0'; return source[current + 1]; }
bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}

void Lexer::addToken(TokenType type) { addToken(type, "", ""); }
void Lexer::addToken(TokenType type, const std::string& literal, const std::string& content) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, content, line});
}

void Lexer::scanString(char quoteType) {
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (!isAtEnd()) advance(); // Closing quote
    addToken(TokenType::STRING);
}

void Lexer::scanNumber() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    addToken(TokenType::NUMBER);
}

void Lexer::scanIdentifier() {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    addToken(type);
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '@': addToken(TokenType::AT); break;
        case '=': addToken(TokenType::EQUAL); break;
        case '+': addToken(TokenType::PLUS); break;
        case '/': addToken(TokenType::SLASH); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '>': addToken(TokenType::GREATER); break;
        case '<': addToken(TokenType::LESS); break;
        case '&': addToken(TokenType::AMPERSAND); break;
        case '.': addToken(TokenType::DOT); break;
        case '#': addToken(TokenType::HASH); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '*': addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
        case '-': addToken(match('-') ? TokenType::GEN_COMMENT : TokenType::MINUS); break;
        case '[': {
            if (match('O')&&match('r')&&match('i')&&match('g')&&match('i')&&match('n')&&match(']')) {
                // --- Special parsing for [Origin] blocks ---
                // Skip whitespace
                while (peek() == ' ' || peek() == '\t') advance();
                // Expect @
                if (!match('@')) { addToken(TokenType::UNKNOWN); break; }
                // Get Type
                int typeStart = current;
                while (isalpha(peek())) advance();
                std::string originType = source.substr(typeStart, current - typeStart);

                // Skip whitespace and find {
                while (peek() == ' ' || peek() == '\t') advance();
                if (!match('{')) { addToken(TokenType::UNKNOWN); break; }

                // Scan for raw content
                int contentStart = current;
                int braceLevel = 1;
                while (braceLevel > 0 && !isAtEnd()) {
                    if (peek() == '{') braceLevel++;
                    else if (peek() == '}') braceLevel--;
                    if (braceLevel > 0) advance();
                }
                std::string content = source.substr(contentStart, current - contentStart);
                match('}'); // consume final brace

                tokens.push_back(Token{TokenType::KEYWORD_ORIGIN, originType, content, line});

            } else if (match('I')&&match('m')&&match('p')&&match('o')&&match('r')&&match('t')&&match(']')) {
                addToken(TokenType::KEYWORD_IMPORT);
            }
            else {
                current = start + 1; // Backtrack
                addToken(TokenType::LEFT_BRACKET);
            }
            break;
        }
        case ' ': case '\r': case '\t': break;
        case '\n': line++; break;
        case '"': case '\'': scanString(c); break;
        default:
            if (isdigit(c)) scanNumber();
            else if (isalpha(c) || c == '_') scanIdentifier();
            else addToken(TokenType::UNKNOWN);
            break;
    }
}
