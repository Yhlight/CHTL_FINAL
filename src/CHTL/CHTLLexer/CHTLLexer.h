#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    void identifier();
    void stringLiteral(char quote);
    void unquotedLiteral();
    void number();
    void handleSpecialSyntax();

    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);

    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    bool isDigit(char c);

    const std::string& source_;
    std::vector<Token> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
    size_t line_ = 1;
    size_t line_start_ = 0;

    static const std::unordered_map<std::string, TokenType> keywords;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
