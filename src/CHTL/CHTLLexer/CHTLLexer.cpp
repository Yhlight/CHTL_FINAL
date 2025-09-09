#include "CHTLLexer.h"
#include <iostream>
#include <unordered_map>

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

// ... helper methods ...
char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::advance() {
    if (!isAtEnd()) {
        if (source[current] == '\n') {
            line++;
            column = 0;
        }
        current++;
        column++;
    }
    return source[current - 1];
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

void CHTLLexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            case '/':
                if (source.size() > current + 1 && source[current + 1] == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (source.size() > current + 1 && source[current + 1] == '*') {
                    advance(); advance(); // Consume /*
                    while (!isAtEnd() && !(peek() == '*' && source[current + 1] == '/')) advance();
                    if (!isAtEnd()) { advance(); advance(); } // Consume */
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token CHTLLexer::makeToken(TokenType type, const std::string& lexeme, size_t startOffset) {
    return {type, lexeme, line, column - (int)lexeme.length(), startOffset};
}

Token CHTLLexer::makeToken(TokenType type, size_t startOffset) {
    return {type, source.substr(startOffset, 1), line, column - 1, startOffset};
}

Token CHTLLexer::errorToken(const std::string& message) {
    return {TokenType::Unknown, message, line, column, current};
}

TokenType CHTLLexer::identifierType(const std::string& identifier) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::Text},
        {"style", TokenType::Style},
        {"Template", TokenType::Identifier}, // Treat "Template" as a regular identifier for the parser
        {"Origin", TokenType::Identifier},   // Treat "Origin" as a regular identifier for the parser
    };
    auto it = keywords.find(identifier);
    return it != keywords.end() ? it->second : TokenType::Identifier;
}

Token CHTLLexer::identifier(size_t startOffset) {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string lexeme = source.substr(startOffset, current - startOffset);
    return makeToken(identifierType(lexeme), lexeme, startOffset);
}

Token CHTLLexer::unquotedLiteral(size_t startOffset) {
    // Stop at any whitespace or symbol that could delimit a value.
    while (!isspace(peek()) && peek() != ';' && peek() != '{' && peek() != '}' &&
           peek() != '(' && peek() != ')' && !isAtEnd()) {
        advance();
    }
    std::string lexeme = source.substr(startOffset, current - startOffset);
    return makeToken(TokenType::UnquotedLiteral, lexeme, startOffset);
}

Token CHTLLexer::stringLiteral(size_t startOffset) {
    char quote_type = advance();
    size_t contentStart = current;
    while (peek() != quote_type && !isAtEnd()) advance();
    if (isAtEnd()) return errorToken("Unterminated string.");
    std::string value = source.substr(contentStart, current - contentStart);
    advance();
    return makeToken(TokenType::StringLiteral, value, startOffset);
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        size_t startOffset = current;
        char c = peek();

        if (isalpha(c) || c == '_') {
            tokens.push_back(identifier(startOffset));
            continue;
        }

        if (c == '"' || c == '\'') {
            tokens.push_back(stringLiteral(startOffset));
            continue;
        }

        if (isdigit(c) || (c == '.' && isdigit(source[current+1]))) {
             tokens.push_back(unquotedLiteral(startOffset));
             continue;
        }

        advance();
        switch (c) {
            case '{': tokens.push_back(makeToken(TokenType::LeftBrace, startOffset)); break;
            case '}': tokens.push_back(makeToken(TokenType::RightBrace, startOffset)); break;
            case '(': tokens.push_back(makeToken(TokenType::LeftParen, startOffset)); break;
            case ')': tokens.push_back(makeToken(TokenType::RightParen, startOffset)); break;
            case '[': tokens.push_back(makeToken(TokenType::LeftBracket, startOffset)); break;
            case ']': tokens.push_back(makeToken(TokenType::RightBracket, startOffset)); break;
            case ':': case '=': tokens.push_back(makeToken(TokenType::Colon, startOffset)); break;
            case ';': tokens.push_back(makeToken(TokenType::Semicolon, startOffset)); break;
            case '@': tokens.push_back(makeToken(TokenType::At, startOffset)); break;
            case '#': tokens.push_back(makeToken(TokenType::Hash, startOffset)); break;
            case '.': tokens.push_back(makeToken(TokenType::Dot, startOffset)); break;
            case ',': tokens.push_back(makeToken(TokenType::Comma, startOffset)); break;
            case '&': tokens.push_back(makeToken(TokenType::Ampersand, startOffset)); break;
            case '>': tokens.push_back(makeToken(TokenType::GreaterThan, startOffset)); break;
            case '<': tokens.push_back(makeToken(TokenType::LessThan, startOffset)); break;
            default:
                // If we are here, it's an unhandled character.
                // For now, assume it might be part of an unquoted literal if it's not whitespace.
                if (!isspace(c)) {
                    current = startOffset;
                    tokens.push_back(unquotedLiteral(startOffset));
                }
                break;
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line, column, current});
    return tokens;
}
