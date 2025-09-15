#ifndef CHTLJSLEXER_H
#define CHTLJSLEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS词法分析器
 * 将源代码分解为Token序列
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer();

    // 词法分析
    std::vector<Token> tokenize(const std::string& source);
    void setSource(const std::string& source);
    
    // 状态管理
    void reset();
    bool hasMoreTokens() const;
    Token nextToken();
    Token peekToken();
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<std::string> errors;
    
    // 辅助方法
    void skipWhitespace();
    void skipComment();
    Token readString();
    Token readNumber();
    Token readIdentifier();
    Token readOperator();
    Token readPunctuation();
    Token readSpecial();
    
    char currentChar() const;
    char peekChar() const;
    void advance();
    bool isAtEnd() const;
    
    // 字符分类
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isOperatorChar(char c) const;
    bool isPunctuationChar(char c) const;
    
    // 关键字识别
    TokenType getKeywordType(const std::string& word) const;
    bool isKeyword(const std::string& word) const;
    
    // 错误报告
    void reportError(const std::string& message);
};

} // namespace CHTLJS

#endif // CHTLJSLEXER_H