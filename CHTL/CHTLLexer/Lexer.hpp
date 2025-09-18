#pragma once

#include "Token.hpp"
#include <string>
#include <map>

namespace CHTL {

class Lexer {
public:
    Lexer(std::string input);

    Token nextToken();

private:
    static const std::map<std::string, TokenType> keywords;
    static TokenType lookupIdent(const std::string& ident);
    void readChar();
    char peekChar();
    void skipWhitespace();
    Token readIdentifier();
    Token readString();
    Token readComment();
    Token readNumber();

    std::string input;
    size_t position;      // current position in input (points to current char)
    size_t readPosition;  // current reading position in input (after current char)
    char ch;              // current char under examination
    int line;
};

} // namespace CHTL
