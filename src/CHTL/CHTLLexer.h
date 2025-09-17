#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    
    // 主要接口
    TokenList tokenize();
    TokenPtr nextToken();
    TokenPtr peekToken(size_t offset = 0);
    bool hasMoreTokens() const;
    
    // 状态管理
    void reset();
    size_t getCurrentPosition() const { return currentPos_; }
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
private:
    std::string source_;
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    TokenList tokens_;
    size_t tokenIndex_;
    
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // 字符处理
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipComment();
    void skipMultilineComment();
    
    // Token识别
    TokenPtr readIdentifier();
    TokenPtr readString();
    TokenPtr readNumber();
    TokenPtr readLiteral();
    TokenPtr readOperator();
    TokenPtr readSymbol();
    TokenPtr readComment();
    TokenPtr readGeneratorComment();
    
    // 辅助方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isStringDelimiter(char c) const;
    bool isCommentStart(char c) const;
    bool isMultilineCommentStart() const;
    bool isGeneratorCommentStart() const;
    
    // 关键字和运算符识别
    TokenType identifyKeyword(const std::string& word);
    TokenType identifyOperator(const std::string& op);
    
    // 错误报告
    void reportError(const std::string& message);
};

} // namespace CHTL

#endif // CHTL_LEXER_H