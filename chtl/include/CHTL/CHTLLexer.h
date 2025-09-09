#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <fstream>
#include <functional>

namespace CHTL {

/**
 * CHTL词法分析器
 * 负责将CHTL源代码转换为Token序列
 */
class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& filename);
    CHTLLexer(const std::string& source, bool isString);
    ~CHTLLexer();
    
    // 词法分析主函数
    std::vector<Token> tokenize();
    
    // 获取当前位置信息
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
private:
    // 输入源
    std::string source_;
    std::ifstream file_;
    bool isFile_;
    
    // 位置信息
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    
    // 状态
    bool inComment_;
    bool inString_;
    char stringDelimiter_;
    
    // 错误处理
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // 核心方法
    void loadSource();
    Token nextToken();
    void skipWhitespace();
    Token parseComment();
    Token parseMultiLineComment();
    Token parseString();
    Token parseNumber();
    Token parseIdentifier();
    Token parseSymbol();
    Token parseLiteral();
    
    // 辅助方法
    char peek() const;
    char advance();
    bool match(char expected);
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isSymbol(char c) const;
    
    // 关键字识别
    TokenType getKeywordType(const std::string& keyword) const;
    bool isKeyword(const std::string& word) const;
    
    // 错误报告
    void reportError(const std::string& message);
};

} // namespace CHTL