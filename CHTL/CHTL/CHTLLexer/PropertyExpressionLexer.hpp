#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CSS属性表达式词法分析器
 * 
 * 专门用于解析CSS属性值中的表达式，包括：
 * - 属性运算：+ - * / % **
 * - 引用属性：选择器.属性
 * - 属性条件表达式：条件 ? 选项 : 选项
 * - 动态属性条件表达式：{{变量}}->属性
 */
class PropertyExpressionLexer {
public:
    PropertyExpressionLexer(const std::string& input);
    
    Token nextToken();
    void ungetToken();
    bool hasMoreTokens() const;
    
    // 获取当前位置信息
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
private:
    std::string input_;
    size_t position_;
    size_t currentLine_;
    size_t currentColumn_;
    std::vector<Token> ungetTokens_;
    
    char getCurrentChar() const;
    char peekNextChar() const;
    void advance();
    void skipWhitespace();
    
    Token parseNumber();
    Token parseString();
    Token parseIdentifier();
    Token parseOperator();
    Token parseSelector();
    
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isOperatorChar(char c) const;
    
    // 特殊解析方法
    Token parsePropertyReference(); // 解析 选择器.属性
    Token parseDynamicReference();  // 解析 {{变量}}->属性
    Token parseConditional();      // 解析 条件 ? 选项 : 选项
};

} // namespace CHTL