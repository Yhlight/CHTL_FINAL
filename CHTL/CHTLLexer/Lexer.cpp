#include "Lexer.h"
#include <cctype>
#include <map>

static const std::map<std::string, TokenType> keywords = {
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT},
    {"top", TokenType::KEYWORD_TOP},
    {"bottom", TokenType::KEYWORD_BOTTOM},
};

Lexer::Lexer(const std::string& source) : source(source), position(0), readPosition(0), ch(0), line(1) {
    readChar();
}

void Lexer::readChar() {
    if (readPosition >= source.length()) {
        ch = 0;
    } else {
        ch = source[readPosition];
    }
    position = readPosition;
    readPosition++;
    if (ch == '\n') {
        line++;
    }
}

char Lexer::peekChar() const {
    if (readPosition >= source.length()) {
        return 0;
    }
    return source[readPosition];
}

void Lexer::skipWhitespace() {
    while (isspace(ch)) {
        readChar();
    }
}

void Lexer::skipSingleLineComment() {
    while (ch != '\n' && ch != 0) {
        readChar();
    }
}

void Lexer::skipMultiLineComment() {
    readChar();
    while (true) {
        if (ch == 0) break;
        if (ch == '*' && peekChar() == '/') {
            readChar(); readChar();
            break;
        }
        readChar();
    }
}

Token Lexer::makeToken(TokenType type, const std::string& literal) {
    return Token{type, literal, line};
}

Token Lexer::readIdentifier() {
    size_t startPos = position;
    while (isalnum(ch) || ch == '_' || ch == '-') {
        readChar();
    }
    std::string literal = source.substr(startPos, position - startPos);
    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        return makeToken(it->second, literal);
    }
    return makeToken(TokenType::IDENTIFIER, literal);
}

Token Lexer::readStringLiteral() {
    char quoteType = ch;
    size_t startPos = position + 1;
    readChar();
    while (ch != quoteType && ch != 0) {
        readChar();
    }
    std::string literal = source.substr(startPos, position - startPos);
    readChar();
    return makeToken(TokenType::STRING_LITERAL, literal);
}

Token Lexer::readHashComment() {
    size_t startPos = position;
    readChar();
    if(isspace(ch)){
         while (ch != '\n' && ch != 0) {
            readChar();
        }
        std::string literal = source.substr(startPos, position - startPos);
        return makeToken(TokenType::HASH_COMMENT, literal);
    }
    return makeToken(TokenType::ILLEGAL, "#");
}

Token Lexer::nextToken() {
    skipWhitespace();
    Token tok;
    switch (ch) {
        case '{': tok = makeToken(TokenType::LEFT_BRACE, "{"); readChar(); break;
        case '}': tok = makeToken(TokenType::RIGHT_BRACE, "}"); readChar(); break;
        case ':': tok = makeToken(TokenType::COLON, ":"); readChar(); break;
        case ';': tok = makeToken(TokenType::SEMICOLON, ";"); readChar(); break;
        case '=': tok = makeToken(TokenType::EQUALS, "="); readChar(); break;
        case '&': tok = makeToken(TokenType::AMPERSAND, "&"); readChar(); break;
        case '.': tok = makeToken(TokenType::DOT, "."); readChar(); break;
        case '@': tok = makeToken(TokenType::AT_SIGN, "@"); readChar(); break;
        case '[': tok = makeToken(TokenType::LEFT_BRACKET, "["); readChar(); break;
        case ']': tok = makeToken(TokenType::RIGHT_BRACKET, "]"); readChar(); break;
        case '(': tok = makeToken(TokenType::LEFT_PAREN, "("); readChar(); break;
        case ')': tok = makeToken(TokenType::RIGHT_PAREN, ")"); readChar(); break;
        case ',': tok = makeToken(TokenType::COMMA, ","); readChar(); break;
        case '"': case '\'': return readStringLiteral();
        case '#':
            if (isspace(peekChar())) { return readHashComment(); }
            else { tok = makeToken(TokenType::POUND, "#"); readChar(); }
            break;
        case '/':
            if (peekChar() == '/') {
                readChar(); readChar(); skipSingleLineComment(); return nextToken();
            } else if (peekChar() == '*') {
                readChar(); readChar(); skipMultiLineComment(); return nextToken();
            } else { tok = makeToken(TokenType::ILLEGAL, std::string(1, ch)); readChar(); }
            break;
        case 0:
            tok = makeToken(TokenType::END_OF_FILE, "");
            break;
        default:
            if (isalnum(ch) || ch == '_') { return readIdentifier(); }
            else { tok = makeToken(TokenType::ILLEGAL, std::string(1, ch)); readChar(); }
            break;
    }
    return tok;
}
