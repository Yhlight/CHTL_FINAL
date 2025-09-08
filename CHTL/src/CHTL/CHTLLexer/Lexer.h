#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    // Constructor initializes the lexer with the source code to be tokenized.
    explicit Lexer(const std::string& source);

    // The main function to call, which processes the entire source and returns a vector of tokens.
    std::vector<Token> tokenize();

private:
    // Core function to get the next token from the source stream.
    Token nextToken();

    // Helper functions for parsing different token types.
    Token makeIdentifierOrUnquotedLiteral();
    Token makeString(char quote_type);
    Token makeToken(TokenType type, const std::string& value = "") const;

    // Utility functions for navigating the source.
    char peek() const;
    char advance();
    bool match(char expected); // New helper function
    void skipLineComment();
    void skipBlockComment();
    Token makeGeneratorComment();

    const std::string& m_source;
    size_t m_start = 0;
    size_t m_current = 0;
    int m_line = 1;
};

} // namespace CHTL
