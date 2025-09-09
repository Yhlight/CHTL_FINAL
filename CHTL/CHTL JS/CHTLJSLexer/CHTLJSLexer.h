#pragma once

#include "CHTLJSToken.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace CHTL {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(std::string source);
    std::vector<CHTLJSToken> tokenize();
    
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<CHTLJSToken> tokens;
    
    // 字符处理
    char currentChar() const;
    char peekChar() const;
    void advance();
    void skipWhitespace();
    void throwError(const std::string& message) const;
    
    // 词法分析
    CHTLJSToken scanString();
    CHTLJSToken scanNumber();
    CHTLJSToken scanIdentifier();
    CHTLJSToken scanOperator();
    CHTLJSToken scanSelector();
    CHTLJSToken scanBoolean();
    
    // 辅助方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphanumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isStringDelimiter(char c) const;
    bool isSelectorStart() const;
    bool isBooleanStart() const;
    
    // 字符分类
    bool isOperatorChar(char c) const;
    bool isDelimiterChar(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
};

} // namespace CHTL