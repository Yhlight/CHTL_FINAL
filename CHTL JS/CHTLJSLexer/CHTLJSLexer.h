#ifndef CHTLJSLEXER_H
#define CHTLJSLEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS词法分析器
 * 负责将CHTL JS代码分解为令牌序列
 */
class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& source);
    ~CHTLJSLexer();
    
    // 主要方法
    std::vector<Token> tokenize();
    Token nextToken();
    bool hasMoreTokens() const;
    
    // 状态查询
    int getCurrentLine() const;
    int getCurrentColumn() const;
    const std::string& getSource() const;

private:
    std::string source_;
    size_t position_;
    int line_;
    int column_;
    
    // 辅助方法
    char currentChar() const;
    char peekChar() const;
    void advance();
    void skipWhitespace();
    void skipComment();
    
    Token parseString();
    Token parseNumber();
    Token parseIdentifier();
    Token parseSelector();
    Token parseResponsiveValue();
    
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    
    TokenType getKeywordType(const std::string& keyword) const;
};

} // namespace CHTLJS

#endif // CHTLJSLEXER_H