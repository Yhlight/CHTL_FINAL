#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        start_column = column;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

bool CHTLLexer::isAtEnd() const {
    return current >= source.length();
}

char CHTLLexer::advance() {
    if (!isAtEnd()) {
        column++;
        return source[current++];
    }
    return '\0';
}

void CHTLLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line, start_column);
}

void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    tokens.emplace_back(type, literal, line, start_column);
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;

        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            column = 1;
            break;

        default:
            if (std::isalpha(c) || c == '_') {
                // For the MVP, any sequence of letters is an unquoted literal/identifier
                handleIdentifier();
            } else if (!isAtEnd()) {
                // Ignore other characters for now
            }
            break;
    }
}

void CHTLLexer::handleIdentifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    // For the MVP, we treat all identifiers as UNQUOTED_LITERAL
    addToken(TokenType::UNQUOTED_LITERAL, text);
}

// A peek function is needed for handleIdentifier
char CHTLLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

} // namespace CHTL
