#include "Lexer.h"
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source), start(0), current(0), line(1) {}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    current++;
    return source[current - 1];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

Token Lexer::makeToken(TokenType type) {
    return Token{type, source.substr(start, current - start), line};
}

Token Lexer::errorToken(const std::string& message) {
    return Token{TokenType::TOKEN_ERROR, message, line};
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isIdentifierChar(char c) {
    return isAlpha(c) || isDigit(c);
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
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    advance(); // Consume /
                    advance(); // Consume *
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (!isAtEnd()) {
                        advance(); // Consume *
                        advance(); // Consume /
                    }
                } else {
                    return; // It's a slash operator, not a comment
                }
                break;
            case '#':
                 while (peek() != '\n' && !isAtEnd()) advance();
                 // We might want to capture the comment value later, but for now, skip.
                 break;
            default:
                return;
        }
    }
}

Token Lexer::identifier() {
    while (isIdentifierChar(peek())) advance();
    // This is a simplified version. We'll need to handle keywords later.
    return makeToken(TokenType::TOKEN_IDENTIFIER);
}

Token Lexer::stringLiteral() {
    char quote_type = source[start]; // " or '
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance(); // The closing quote
    return makeToken(TokenType::TOKEN_STRING_LITERAL);
}


Token Lexer::nextToken() {
    skipWhitespaceAndComments();
    start = current;

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) {
        // For now, CHTL doesn't seem to have number literals separate from unquoted literals.
        // We can treat them as part of unquoted literals.
        // Let's backtrack and handle it there.
        current--; // backtrack
    }


    switch (c) {
        case '{': return makeToken(TokenType::TOKEN_LEFT_BRACE);
        case '}': return makeToken(TokenType::TOKEN_RIGHT_BRACE);
        case ':': return makeToken(TokenType::TOKEN_COLON);
        case '=': return makeToken(TokenType::TOKEN_EQUALS);
        case ';': return makeToken(TokenType::TOKEN_SEMICOLON);
        case '[': return makeToken(TokenType::TOKEN_LEFT_BRACKET);
        case ']': return makeToken(TokenType::TOKEN_RIGHT_BRACKET);
        case '@': return makeToken(TokenType::TOKEN_AT);
        case '$': return makeToken(TokenType::TOKEN_DOLLAR);
        case '&': return makeToken(TokenType::TOKEN_AMPERSAND);
        case '+': return makeToken(TokenType::TOKEN_PLUS);
        case '-': return makeToken(TokenType::TOKEN_MINUS);
        case '*': return makeToken(TokenType::TOKEN_STAR);
        case '/': return makeToken(TokenType::TOKEN_SLASH);
        case '%': return makeToken(TokenType::TOKEN_PERCENT);
        case '"':
        case '\'':
            return stringLiteral();
    }

    // Unquoted literals are tricky. They can contain almost anything
    // until a character that signifies the end of the literal, like ;, {, or }.
    // The parser will have a better context for this.
    // A simple lexer might just consume until a known delimiter.
    while (!isAtEnd() && peek() != ';' && peek() != '{' && peek() != '}' && peek() != '\n' && peek() != ':') {
        advance();
    }
    // Let's refine this. The logic for unquoted literals might be better handled by just consuming until a delimiter.
    // For now, if it's not anything else, let's just make an error.
    // The parser will drive the need for unquoted literals.
    // The simple definition is "anything that is not a quoted string".
    // A better approach for unquoted is to consume until a delimiter recognized by the parser.
    // Let's try a simple version.
    current--;
    while (!isAtEnd()) {
        char ch = peek();
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' ||
            ch == '{' || ch == '}' || ch == ':' || ch == ';' || ch == '=') {
            break;
        }
        advance();
    }

    if (start == current) {
         return errorToken("Unexpected character.");
    }

    // We consumed something. Let's check if it's not empty.
    std::string value = source.substr(start, current - start);
    if (!value.empty()) {
        return makeToken(TokenType::TOKEN_UNQUOTED_LITERAL);
    }


    return errorToken("Unexpected character.");
}
