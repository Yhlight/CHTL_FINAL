#include "CHTLLexer.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

namespace CHTL {

static std::unordered_map<std::string, TokenType> keywords = {
    {"style", TokenType::STYLE},
    {"text", TokenType::TEXT}
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

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;

        // Ignore whitespace.
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
            } else if (peek() == '*') {
                // Multi-line comment
                advance(); // Consume the '*'
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }

                if (isAtEnd()) {
                    std::cout << "Line " << line << ": Unterminated multi-line comment." << std::endl;
                } else {
                    // Consume the "*/"
                    advance();
                    advance();
                }
            }
            // Note: A single '/' is not a token in CHTL for now.
            break;

        case '"':
        case '\'':
            string(c);
            break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                // Instead of erroring, treat other single characters as symbols.
                // This is more flexible for things like CSS properties and values.
                addToken(TokenType::SYMBOL);
            }
            break;
    }
}

char CHTLLexer::advance() {
    return source[current++];
}

void CHTLLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line});
}

char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void CHTLLexer::string(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        std::cout << "Line " << line << ": Unterminated string." << std::endl;
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLLexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void CHTLLexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

} // namespace CHTL
