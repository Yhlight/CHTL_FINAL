#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include "Token.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTLJS {

class CHTLJSLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    size_t start;
    
    std::unordered_map<std::string, TokenType> keywords;
    std::unordered_map<std::string, TokenType> operators;
    
    void initializeKeywords();
    void initializeOperators();
    
    char current() const;
    char peek(size_t offset = 1) const;
    void advance();
    bool isAtEnd() const;
    
    void skipWhitespace();
    void skipComment();
    void skipLineComment();
    void skipBlockComment();
    
    Token scanString();
    Token scanNumber();
    Token scanIdentifier();
    Token scanOperator();
    Token scanSelector();
    Token scanReactiveValue();
    
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    Token makeToken(TokenType type, const std::string& value = "");
    Token makeToken(TokenType type, char value);
    
    void error(const std::string& message) const;
    
public:
    CHTLJSLexer(const std::string& source);
    ~CHTLJSLexer() = default;
    
    TokenList tokenize();
    TokenList tokenize(const std::string& source);
    
    void reset();
    void setSource(const std::string& source);
    
    std::string getSource() const;
    size_t getPosition() const;
    size_t getLine() const;
    size_t getColumn() const;
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H