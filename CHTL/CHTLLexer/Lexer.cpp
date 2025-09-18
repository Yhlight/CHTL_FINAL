#include "Lexer.hpp"
#include <cctype>

namespace CHTL {

const std::map<std::string, TokenType> Lexer::keywords = {
    {"inherit", TokenType::INHERIT},
};

TokenType Lexer::lookupIdent(const std::string& ident) {
    if (keywords.count(ident)) {
        return keywords.at(ident);
    }
    return TokenType::IDENT;
}

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
    if (std::isalpha(ch) || ch == '_') {
        readChar();
        while (std::isalnum(ch) || ch == '_' || ch == '-') {
            readChar();
        }
    }
    std::string literal = input.substr(startPosition, position - startPosition);
    TokenType type = lookupIdent(literal);
    return {type, literal, line};
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
        case '[':
            tok = {TokenType::LBRACKET, "[", line};
            break;
        case ']':
            tok = {TokenType::RBRACKET, "]", line};
            break;
        case '@':
            tok = {TokenType::AT, "@", line};
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
                return readIdentifier();
            } else {
                tok = {TokenType::ILLEGAL, std::string(1, ch), line};
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
