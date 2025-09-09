#pragma once

#include "CHTLJSToken.hpp"
#include <string>
#include <istream>

namespace CHTL {

/**
 * @brief CHTL JS 词法分析器
 * 
 * 根据 CHTL.md 文档第 1200-1800 行实现
 * 支持 CHTL JS 扩展语法的词法分析
 */
class CHTLJSLexer {
public:
    CHTLJSLexer(std::istream& input);
    
    // 获取下一个词法单元
    CHTLJSToken nextToken();
    
    // 预览下一个词法单元
    CHTLJSToken peekToken();
    
    // 获取当前词法单元
    CHTLJSToken currentToken() const { return current_; }
    
    // 检查是否到达文件末尾
    bool isAtEnd() const;
    
private:
    std::istream& input_;
    CHTLJSToken current_;
    size_t line_;
    size_t column_;
    
    // 读取字符
    char readChar();
    char peekChar();
    
    // 跳过空白字符
    void skipWhitespace();
    
    // 跳过注释
    void skipComment();
    
    // 读取标识符
    CHTLJSToken readIdentifier();
    
    // 读取字符串字面量
    CHTLJSToken readStringLiteral();
    
    // 读取无修饰字面量
    CHTLJSToken readUnquotedLiteral();
    
    // 读取数字字面量
    CHTLJSToken readNumberLiteral();
    
    // 读取增强选择器
    CHTLJSToken readEnhancedSelector();
    
    // 检查是否为关键字
    CHTLJSTokenType checkKeyword(const std::string& word);
    
    // 检查是否为操作符
    CHTLJSTokenType checkOperator(char c);
    
    // 创建词法单元
    CHTLJSToken makeToken(CHTLJSTokenType type, const std::string& value);
    
    // 创建错误词法单元
    CHTLJSToken makeError(const std::string& message);
};

} // namespace CHTL