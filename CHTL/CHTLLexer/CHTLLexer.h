#ifndef CHTLLEXER_H
#define CHTLLEXER_H

#include "CHTLToken.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL词法分析器
 * 将CHTL源代码分解为Token序列
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();

    // 词法分析
    std::vector<CHTLToken> tokenize(const std::string& source);
    void setSource(const std::string& source);
    
    // 状态管理
    void reset();
    bool hasMoreTokens() const;
    CHTLToken nextToken();
    CHTLToken peekToken();
    
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
    CHTLToken readString();
    CHTLToken readNumber();
    CHTLToken readIdentifier();
    CHTLToken readOperator();
    CHTLToken readPunctuation();
    CHTLToken readSpecial();
    CHTLToken readGeneratorComment();
    
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
    CHTLTokenType getKeywordType(const std::string& word) const;
    CHTLTokenType getHTMLTagType(const std::string& word) const;
    CHTLTokenType getCHTLKeywordType(const std::string& word) const;
    bool isKeyword(const std::string& word) const;
    bool isHTMLTag(const std::string& word) const;
    bool isCHTLKeyword(const std::string& word) const;
    
    // 错误报告
    void reportError(const std::string& message);
};

} // namespace CHTL

#endif // CHTLLEXER_H