#pragma once

#include "Token.h"
#include <string>
#include <cctype>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& input) : input(input), position(0), readPosition(0), ch(0) {
        readChar(); // Initialize the lexer with the first character
    }

    Token NextToken() {
        Token tok;

        skipWhitespace();

        switch (ch) {
            case '{':
                tok = {TokenType::LBRACE, "{"};
                break;
            case '}':
                tok = {TokenType::RBRACE, "}"};
                break;
            case '"':
            case '\'':
                return readString(ch);
            case 0:
                tok = {TokenType::END_OF_FILE, ""};
                break;
            default:
                // For CHTL, identifiers and unquoted literals can be a wide range of characters
                if (isIdentifierStart(ch)) {
                    return readIdentifier();
                } else {
                    tok = {TokenType::ILLEGAL, std::string(1, ch)};
                }
                break;
        }

        readChar(); // Advance to the next character
        return tok;
    }

private:
    void readChar() {
        if (readPosition >= input.length()) {
            ch = 0; // EOF
        } else {
            ch = input[readPosition];
        }
        position = readPosition;
        readPosition++;
    }

    void skipWhitespace() {
        while (isspace(ch)) {
            readChar();
        }
    }

    // For now, an identifier is any sequence of non-special, non-whitespace characters.
    // This is a simple but effective starting point for CHTL's flexible syntax.
    bool isIdentifierStart(char c) const {
        return c != 0 && !isspace(c) && c != '{' && c != '}' && c != '"' && c != '\'';
    }

    Token readIdentifier() {
        size_t startPosition = position;
        while (isIdentifierStart(ch)) {
            readChar();
        }
        return {TokenType::IDENTIFIER, input.substr(startPosition, position - startPosition)};
    }

    Token readString(char quote_type) {
        size_t startPosition = position + 1; // Skip the opening quote
        readChar(); // Consume opening quote

        while (ch != quote_type && ch != 0) {
            readChar();
        }

        std::string value = input.substr(startPosition, position - startPosition);

        if (ch == 0) { // Unterminated string
            return {TokenType::ILLEGAL, value};
        }

        readChar(); // Consume closing quote
        return {TokenType::STRING, value};
    }

    const std::string input;
    size_t position;      // current position in input (points to current char)
    size_t readPosition;  // current reading position in input (after current char)
    char ch;              // current char under examination
};

} // namespace CHTL
