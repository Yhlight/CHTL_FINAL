#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;
    
    // 主要词法分析函数
    std::vector<Token> tokenize(const std::string& sourceCode);
    
    // 配置
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setCaseSensitive(bool enabled) { caseSensitive_ = enabled; }
    
    // 错误处理
    bool hasErrors() const { return !errors_.empty(); }
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }

private:
    // 状态
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    std::vector<std::string> errors_;
    
    // 配置
    bool debugMode_;
    bool caseSensitive_;
    
    // 核心词法分析函数
    Token getNextToken();
    Token readIdentifier();
    Token readString();
    Token readNumber();
    Token readLiteral();
    Token readComment();
    Token readMultilineComment();
    Token readGeneratorComment();
    Token readSymbol();
    Token readKeyword();
    
    // 辅助函数
    char currentChar() const;
    char peekChar() const;
    char advance();
    void skipWhitespace();
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isSymbol(char c) const;
    bool isStringDelimiter(char c) const;
    bool isCommentStart(char c) const;
    bool isGeneratorCommentStart(char c) const;
    
    // 错误处理
    void addError(const std::string& message);
    void addError(const std::string& message, size_t line, size_t column);
    
    // 调试
    void debugToken(const Token& token) const;
};

} // namespace CHTL