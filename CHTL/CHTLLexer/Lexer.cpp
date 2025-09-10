#include "Lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source), start(0), current(0), line(1) {}

bool Lexer::isAtEnd() { return current >= source.length(); }
char Lexer::advance() { current++; return source[current - 1]; }
char Lexer::peek() { if (isAtEnd()) return '\0'; return source[current]; }
char Lexer::peekNext() { if (current + 1 >= source.length()) return '\0'; return source[current + 1]; }
bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}
Token Lexer::makeToken(TokenType type) { return Token{type, source.substr(start, current - start), line, start}; }
Token Lexer::errorToken(const std::string& message) { return Token{TokenType::TOKEN_UNKNOWN, message, line, start}; }

void Lexer::skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ': case '\r': case '\t': advance(); break;
            case '\n': line++; advance(); break;
            case '/':
                if (peekNext() == '/') { while (peek() != '\n' && !isAtEnd()) advance(); }
                else if (peekNext() == '*') {
                    advance(); advance();
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (!isAtEnd()) advance();
                    if (!isAtEnd()) advance();
                } else { return; }
                break;
            default: return;
        }
    }
}

Token Lexer::stringToken() {
    char quote = source[start];
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string.");
    advance();
    return makeToken(TokenType::TOKEN_STRING);
}

Token Lexer::numberToken() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    return makeToken(TokenType::TOKEN_NUMBER);
}

TokenType checkKeyword(const std::string& text) {
    if (text == "text") return TokenType::TOKEN_KEYWORD_TEXT;
    if (text == "from") return TokenType::KEYWORD_FROM;
    if (text == "as") return TokenType::KEYWORD_AS;
    if (text == "delete") return TokenType::KEYWORD_DELETE;
    return TokenType::TOKEN_IDENTIFIER;
}

Token Lexer::identifierToken() {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string text = source.substr(start, current - start);
    return makeToken(checkKeyword(text));
}

Token Lexer::commentToken() {
    while (peek() != '\n' && !isAtEnd()) advance();
    return makeToken(TokenType::TOKEN_GENERATOR_COMMENT);
}

Token Lexer::nextToken() {
    skipWhitespace();
    start = current;
    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    // Handle bracketed keywords first
    if (peek() == '[') {
        if (source.substr(current, 8) == "[Origin]") {
            current += 8;
            return makeToken(TokenType::KEYWORD_ORIGIN);
        }
        if (source.substr(current, 11) == "[Namespace]") {
            current += 11;
            return makeToken(TokenType::KEYWORD_NAMESPACE);
        }
        if (source.substr(current, 8) == "[Import]") {
            current += 8;
            return makeToken(TokenType::KEYWORD_IMPORT);
        }
        if (source.substr(current, 10) == "[Template]") {
            current += 10;
            return makeToken(TokenType::KEYWORD_TEMPLATE);
        }
        if (source.substr(current, 8) == "[Custom]") {
            current += 8;
            return makeToken(TokenType::KEYWORD_CUSTOM);
        }
    }


    char c = advance();

    if (isalpha(c) || c == '_') return identifierToken();
    if (isdigit(c)) return numberToken();

    switch (c) {
        case '(': return makeToken(TokenType::TOKEN_LPAREN);
        case ')': return makeToken(TokenType::TOKEN_RPAREN);
        case '{': return makeToken(TokenType::TOKEN_LBRACE);
        case '}': return makeToken(TokenType::TOKEN_RBRACE);
        case ':': return makeToken(TokenType::TOKEN_COLON);
        case ';': return makeToken(TokenType::TOKEN_SEMICOLON);
        case '.': return makeToken(TokenType::TOKEN_DOT);
        case '#': return makeToken(TokenType::TOKEN_HASH);
        case '@': return makeToken(TokenType::TOKEN_AT);
        case '+': return makeToken(TokenType::TOKEN_PLUS);
        case '/': return makeToken(TokenType::TOKEN_SLASH);
        case '%': return makeToken(TokenType::TOKEN_PERCENT);
        case '?': return makeToken(TokenType::TOKEN_QUESTION);
        case '"': case '\'': return stringToken();
        case '-':
            if (match('-')) return commentToken();
            if (isalpha(peek())) {
                 current--;
                 return identifierToken();
            }
            return makeToken(TokenType::TOKEN_MINUS);
        case '*': return makeToken(match('*') ? TokenType::TOKEN_STAR_STAR : TokenType::TOKEN_STAR);
        case '=': return makeToken(match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
        case '!': return makeToken(match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_UNKNOWN);
        case '<': return makeToken(match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
        case '>': return makeToken(match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
        case '&': return makeToken(match('&') ? TokenType::TOKEN_AMPERSAND_AMPERSAND : TokenType::TOKEN_AMPERSAND);
        case '|': return makeToken(match('|') ? TokenType::TOKEN_PIPE_PIPE : TokenType::TOKEN_UNKNOWN);
    }

    return errorToken("Unexpected character.");
}
