#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    
    // 获取下一个 token
    std::unique_ptr<Token> nextToken();
    
    // 预览下一个 token 但不消费
    std::unique_ptr<Token> peekToken();
    
    // 检查是否还有更多 token
    bool hasMoreTokens() const;
    
    // 获取当前位置
    size_t getCurrentPosition() const { return current_pos_; }
    size_t getCurrentLine() const { return current_line_; }
    size_t getCurrentColumn() const { return current_column_; }
    
    // 重置到指定位置
    void reset(size_t position);
    
private:
    std::string source_;
    size_t current_pos_;
    size_t current_line_;
    size_t current_column_;
    
    // 跳过空白字符
    void skipWhitespace();
    
    // 跳过注释
    void skipComment();
    
    // 读取字符串
    std::string readString(char delimiter);
    
    // 读取数字
    std::string readNumber();
    
    // 读取标识符或关键字
    std::string readIdentifier();
    
    // 读取字面量
    std::string readLiteral();
    
    // 检查是否是字母
    bool isAlpha(char c) const;
    
    // 检查是否是数字
    bool isDigit(char c) const;
    
    // 检查是否是字母或数字
    bool isAlphaNumeric(char c) const;
    
    // 检查是否是空白字符
    bool isWhitespace(char c) const;
    
    // 获取当前字符
    char currentChar() const;
    
    // 获取下一个字符
    char nextChar();
    
    // 预览下一个字符
    char peekChar() const;
    
    // 检查是否是关键字
    bool isKeyword(const std::string& word) const;
    
    // 检查是否是特殊符号
    TokenType getSpecialSymbol(const std::string& symbol) const;
};

} // namespace CHTL