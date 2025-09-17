#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace CHTLJS {

/**
 * CHTL JS词法分析器
 * 专门为CHTL JS语言设计，不是JavaScript词法分析器
 */
class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& source);
    ~CHTLJSLexer();
    
    // 主要方法
    std::vector<CHTLJS_Token> tokenize();
    CHTLJS_Token nextToken();
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
    
    // 解析方法
    CHTLJS_Token parseString();
    CHTLJS_Token parseNumber();
    CHTLJS_Token parseIdentifier();
    CHTLJS_Token parseCHTLSelector();
    CHTLJS_Token parseCHTLResponsive();
    CHTLJS_Token parseOperator();
    CHTLJS_Token parsePunctuation();
    
    // 字符检查
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // 关键字和操作符识别
    CHTLJS_TokenType getKeywordType(const std::string& keyword) const;
    CHTLJS_TokenType getOperatorType(const std::string& op) const;
    CHTLJS_TokenType getPunctuationType(const std::string& punct) const;
    
    // 静态映射
    static const std::map<std::string, CHTLJS_TokenType> keywords_;
    static const std::map<std::string, CHTLJS_TokenType> operators_;
    static const std::map<std::string, CHTLJS_TokenType> punctuation_;
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H