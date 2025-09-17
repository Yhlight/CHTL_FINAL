#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>

namespace CHTL {

class Lexer {
public:
    Lexer(std::string input);

    // Returns the next token from the input stream.
    Token nextToken();

private:
    // Helper methods for reading characters and advancing the cursor.
    void readChar();
    char peekChar() const;

    // Helper to consume whitespace.
    void skipWhitespace();

    // Helper methods for reading specific types of tokens.
    Token readIdentifier();
    Token readString();
    Token readQuotedString(char quote_type);

    // Checks if a character can be part of an identifier.
    // CHTL identifiers can contain letters, numbers, and hyphens.
    bool isIdentifierChar(char c) const;

    std::string m_input;
    size_t m_position = 0;      // Current character position (points to current char)
    size_t m_readPosition = 0;  // Next character position (points to next char)
    char m_ch = 0;              // Current character under examination
    int m_line = 1;             // Current line number for error reporting
};

} // namespace CHTL

#endif // LEXER_H
