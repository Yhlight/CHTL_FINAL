#include "CHTLLexer.h"
#include <map>
#include <cctype>
#include <iostream>

namespace CHTL {

static std::map<std::string, TokenType> keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE}
};

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

char CHTLLexer::advance() {
    return source[current++];
}

char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void CHTLLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    // The literal value is stored in the lexeme field for simplicity.
    tokens.push_back({type, literal, line});
}

void CHTLLexer::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        // In a real compiler, we would report an error.
        std::cerr << "Line " << line << ": Unterminated string." << std::endl;
        return;
    }

    // Consume the closing ".
    advance();

    // Trim the surrounding quotes to get the literal value.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLLexer::identifier() {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '=': addToken(TokenType::EQUAL); break;
        case '&': addToken(TokenType::AMPERSAND); break;

        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;

        case '/':
            if (peek() == '/') {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                // A single slash is not a valid token in this context
                std::cerr << "Line " << line << ": Unexpected character '" << c << "'." << std::endl;
                addToken(TokenType::UNKNOWN);
            }
            break;

        case '"':
            stringLiteral();
            break;

        case '.':
        case '#': {
            char startChar = source[start];
            // Consume the identifier part of the selector
            while (isalnum(peek()) || peek() == '_' || peek() == '-') {
                advance();
            }
            // If we only consumed the '.' or '#', it's an error, but for now, we'll tokenize it.
            // A better parser would validate this.
            addToken(startChar == '.' ? TokenType::CLASS_SELECTOR : TokenType::ID_SELECTOR);
            break;
        }

        default:
            if (isdigit(c)) {
                numberLiteral();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                std::cerr << "Line " << line << ": Unexpected character '" << c << "'." << std::endl;
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}

void CHTLLexer::numberLiteral() {
    while (isdigit(peek())) {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) {
            advance();
        }
    }

    // Also consume an attached unit (e.g., px, em, %)
    while (isalpha(peek()) || peek() == '%') {
        advance();
    }

    addToken(TokenType::NUMBER);
}

} // namespace CHTL
