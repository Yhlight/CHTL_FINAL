#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL Lexer
 * 
 * Implements syntax specification from CHTL.md lines 4-79
 * Supports:
 * - Comments: //, /* *\/, --
 * - Text nodes: text { }
 * - Literals: unquoted literals, double quotes, single quotes
 * - Element nodes: All HTML elements
 * - Attributes: attribute_name : "attribute_value"
 * - CE equivalence: : and = are equivalent
 */
class Lexer {
public:
    explicit Lexer(const std::string& source);
    
    // 获取下一个词法单元
    Token nextToken();
    
    // 查看下一个词法单元（不消费）
    Token peekToken();
    
    // 查看下下个词法单元
    Token peekNextToken();
    
    // 重置到开始位置
    void reset();
    
    // 获取当前位置
    size_t getCurrentPosition() const { return current_; }
    
    // 获取当前行号
    size_t getCurrentLine() const { return line_; }
    
    // 获取当前列号
    size_t getCurrentColumn() const { return column_; }
    
    // 获取所有词法单元
    std::vector<Token> tokenize();
    
private:
    // 跳过空白字符
    void skipWhitespace();
    
    // 跳过注释
    void skipComment();
    
    // 跳过单行注释
    void skipSingleLineComment();
    
    // 跳过多行注释
    void skipMultiLineComment();
    
    // 跳过生成器注释
    void skipGeneratorComment();
    
    // 读取字符串字面量
    Token readStringLiteral();
    
    // 读取无修饰字面量
    Token readUnquotedLiteral();
    
    // 读取数字
    Token readNumber();
    
    // 读取标识符或关键字
    Token readIdentifier();
    
    // 读取操作符
    Token readOperator();
    
    // 读取模板关键字
    Token readTemplateKeyword();
    
    // 读取方括号关键字
    Token readBracketKeyword();
    
    // 读取增强选择器
    Token readEnhancedSelector();
    
    // 检查是否为字母
    bool isAlpha(char c) const;
    
    // 检查是否为数字
    bool isDigit(char c) const;
    
    // 检查是否为字母或数字
    bool isAlphaNumeric(char c) const;
    
    // 检查是否为空白字符
    bool isWhitespace(char c) const;
    
    // 检查是否为引号
    bool isQuote(char c) const;
    
    // 获取当前字符
    char currentChar() const;
    
    // 获取下一个字符
    char nextChar();
    
    // 回退一个字符
    void ungetChar();
    
    // 创建词法单元
    Token createToken(TokenType type, const std::string& value);
    
    // 创建错误词法单元
    Token createErrorToken(const std::string& message);
    
private:
    std::string source_;
    size_t current_;
    size_t line_;
    size_t column_;
    size_t start_;
    size_t startLine_;
    size_t startColumn_;
};

} // namespace CHTL