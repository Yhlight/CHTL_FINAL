#include "Lexer.hpp"
#include <cctype>

namespace CHTL {

Lexer::Lexer(std::string input) : input(std::move(input)), position(0), readPosition(0), ch(0), line(1) {
    readChar(); // Initialize the first character
}

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0; // EOF
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition += 1;
}

char Lexer::peekChar() {
    if (readPosition >= input.length()) {
        return 0;
    } else {
        return input[readPosition];
    }
}

void Lexer::skipWhitespace() {
    while (true) {
        if (std::isspace(ch)) {
            if (ch == '\n') {
                line++;
            }
            readChar();
        } else if (ch == '/' && peekChar() == '/') {
            // Single line comment
            while (ch != '\n' && ch != 0) {
                readChar();
            }
        } else if (ch == '/' && peekChar() == '*') {
            // Multi-line comment
            readChar(); // consume '/'
            readChar(); // consume '*'
            while (ch != 0 && (ch != '*' || peekChar() != '/')) {
                if (ch == '\n') {
                    line++;
                }
                readChar();
            }
            readChar(); // consume '*'
            readChar(); // consume '/'
        } else {
            break;
        }
    }
}

Token Lexer::readIdentifier() {
    size_t startPosition = position;
    // CHTL identifiers can be simple names for tags or properties
    // Allow letters, numbers, underscore, and hyphen (but not as the first char for numbers)
    if (std::isalpha(ch) || ch == '_') {
        readChar();
        while (std::isalnum(ch) || ch == '_' || ch == '-') {
            readChar();
        }
    }
    return {TokenType::IDENT, input.substr(startPosition, position - startPosition), line};
}

Token Lexer::readString() {
    size_t startPosition = position + 1; // Skip the opening "
    readChar(); // Consume the opening "
    while (ch != '"' && ch != 0) {
        readChar();
    }
    std::string literal = input.substr(startPosition, position - startPosition);
    readChar(); // Consume the closing "
    return {TokenType::STRING, literal, line};
}

Token Lexer::readComment() {
    readChar(); // Consume '#'
    if (ch == ' ') {
        readChar(); // Consume space
    }
    size_t startPosition = position;
    while (ch != '\n' && ch != 0) {
        readChar();
    }
    return {TokenType::COMMENT, input.substr(startPosition, position - startPosition), line};
}

Token Lexer::nextToken() {
    Token tok;

    skipWhitespace();

    switch (ch) {
        case '{':
            tok = {TokenType::LBRACE, "{", line};
            break;
        case '}':
            tok = {TokenType::RBRACE, "}", line};
            break;
        case ':':
            tok = {TokenType::COLON, ":", line};
            break;
        case '=':
            tok = {TokenType::EQUAL, "=", line};
            break;
        case ';':
            tok = {TokenType::SEMICOLON, ";", line};
            break;
        case '#':
            return readComment();
        case '"':
            return readString();
        case 0:
            tok = {TokenType::END_OF_FILE, "", line};
            break;
        default:
            if (std::isalpha(ch) || ch == '_') {
                // It's an identifier
                return readIdentifier();
            } else {
                // For now, treat unquoted text as a series of identifiers.
                // A more advanced implementation would handle this differently,
                // likely with guidance from the parser.
                // Let's handle the simple case of unquoted text inside text block
                // by treating it as an IDENT. The parser can concatenate them.
                // For now, if it is not an alphabet, it is illegal.
                tok = {TokenType::ILLEGAL, std::string(1, ch), line};
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
