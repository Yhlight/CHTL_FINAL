#pragma once

#include <string>

namespace CHTL {

/**
 * @brief CHTL JS 词法单元类型
 * 
 * 根据 CHTL.md 文档第 1200-1800 行实现
 * 支持 CHTL JS 扩展语法
 */
enum class CHTLJSTokenType {
    // 基础类型
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMBER_LITERAL,
    
    // 操作符
    ASSIGN,         // =
    COLON,          // :
    COMMA,          // ,
    SEMICOLON,      // ;
    DOT,            // .
    PIPE,           // |
    
    // 括号
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    
    // CHTL JS 特定语法
    FILELOADER,     // fileloader
    SCRIPT,         // script
    ENHANCED_SELECTOR_START,  // {{
    ENHANCED_SELECTOR_END,    // }}
    REFERENCE,      // &
    ANIMATE,        // animate
    LISTEN,         // listen
    UTIL,           // util
    CHANGE,         // change
    THEN,           // then
    ARROW,          // ->
    
    // 关键字
    IF,
    ELSE,
    FOR,
    WHILE,
    FUNCTION,
    RETURN,
    VAR,
    LET,
    CONST,
    
    // 特殊
    END_OF_FILE,
    ERROR
};

/**
 * @brief CHTL JS 词法单元
 */
class CHTLJSToken {
public:
    CHTLJSToken(CHTLJSTokenType type, const std::string& value, size_t line, size_t column);
    CHTLJSToken();
    
    CHTLJSTokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    void setType(CHTLJSTokenType type) { type_ = type; }
    void setValue(const std::string& value) { value_ = value; }
    void setLine(size_t line) { line_ = line; }
    void setColumn(size_t column) { column_ = column; }
    
    std::string toString() const;
    
private:
    CHTLJSTokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

} // namespace CHTL