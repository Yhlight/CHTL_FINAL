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
                advance();
                break;
            case '\n':
                advance();
                break; // line/col are handled in advance()
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
    };
    auto it = keywords.find(identifier);
    return it != keywords.end() ? it->second : TokenType::Identifier;
}

Token CHTLLexer::identifier(size_t startOffset) {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance(); // Allow hyphens
    std::string lexeme = source.substr(startOffset, current - startOffset);
    return makeToken(identifierType(lexeme), lexeme, startOffset);
}

Token CHTLLexer::unquotedLiteral(size_t startOffset) {
    while (peek() != ';' && peek() != '{' && peek() != '}' && peek() != '\n' && !isAtEnd()) advance();
    std::string lexeme = source.substr(startOffset, current - startOffset);
    const std::string whitespace = " \t";
    size_t first = lexeme.find_first_not_of(whitespace);
    if (std::string::npos == first) return makeToken(TokenType::UnquotedLiteral, "", startOffset);
    size_t last = lexeme.find_last_not_of(whitespace);
    return makeToken(TokenType::UnquotedLiteral, lexeme.substr(first, (last - first + 1)), startOffset);
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

        if (c == '[') {
            if (source.substr(current, 10) == "[Template]") {
                current += 10;
                tokens.push_back(makeToken(TokenType::TemplateKeyword, "[Template]", startOffset));
                continue;
            }
        }

        if (isalpha(c) || c == '_') {
            tokens.push_back(identifier(startOffset));
            continue;
        }

        if (c == '"' || c == '\'') {
            tokens.push_back(stringLiteral(startOffset));
            continue;
        }

        advance(); // Consume the character for the switch
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
                tokens.push_back(unquotedLiteral(startOffset));
                break;
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line, column, current});
    return tokens;
}
