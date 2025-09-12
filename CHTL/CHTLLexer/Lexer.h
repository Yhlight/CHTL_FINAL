#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL
{
    class Lexer
    {
    public:
        Lexer(std::string input);
        std::vector<Token> tokenize();

    private:
        void advance();
        char peek();
        char peekNext();
        Token makeToken(TokenType type, const std::string& literal);
        Token identifier();
        Token stringLiteral(char quoteType);
        Token numberLiteral();
        Token comment();
        void skipWhitespace();

        std::string m_input;
        size_t m_position;
        int m_line;
        char m_char; // current character
    };
}

#endif // CHTL_LEXER_H
