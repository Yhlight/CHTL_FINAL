#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL 词法单元类型
 * 
 * 根据 CHTL.md 文档定义的语法元素
 */
enum class TokenType {
    // 基础符号
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COLON,          // :
    EQUAL,          // =
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    AT,             // @
    AMPERSAND,      // &
    QUESTION,       // ?
    EXCLAMATION,    // !
    UNDERSCORE,     // _
    SLASH,          // /
    BACKSLASH,      // backslash
    PIPE,           // |
    TILDE,          // ~
    CARET,          // ^
    DOLLAR,         // $
    PERCENT,        // %
    
    // 算术运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    
    // 比较运算符
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    
    // 逻辑运算符
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    LOGICAL_NOT,    // !
    
    // 注释
    SINGLE_COMMENT, // //
    MULTI_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
    // 字符串
    STRING_LITERAL, // "string" 或 'string'
    UNQUOTED_LITERAL, // 无修饰字面量
    
    // 关键字
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    USE,            // use
    EXCEPT,         // except
    DELETE,         // delete
    INSERT,         // insert
    INHERIT,        // inherit
    FROM,           // from
    AS,             // as
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    HTML5,          // html5
    
    // 模板类型
    TEMPLATE_STYLE, // @Style
    TEMPLATE_ELEMENT, // @Element
    TEMPLATE_VAR,   // @Var
    ORIGIN_HTML,    // @Html
    ORIGIN_STYLE,   // @Style
    ORIGIN_JAVASCRIPT, // @JavaScript
    ORIGIN_CHTL,    // @Chtl
    ORIGIN_CJMOD,   // @CJmod
    
    // 标识符
    IDENTIFIER,     // 标识符
    NUMBER,         // 数字
    
    // 特殊符号
    ARROW,          // ->
    DOUBLE_COLON,   // ::
    TRIPLE_DOT,     // ...
    
    // 文件结束
    END_OF_FILE,    // EOF
    
    // 错误
    ERROR           // 错误标记
};

/**
 * @brief CHTL 词法单元
 */
class Token {
public:
    Token() : type_(TokenType::END_OF_FILE), line_(0), column_(0) {}
    Token(TokenType type, std::string_view value, size_t line, size_t column)
        : type_(type), value_(value), line_(line), column_(column) {}
    
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isComment() const;
    
    std::string toString() const;
    
private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

/**
 * @brief 关键字映射表
 */
class KeywordMap {
public:
    static const std::unordered_map<std::string, TokenType>& getKeywords();
    static TokenType getKeywordType(const std::string& keyword);
    static bool isKeyword(const std::string& word);
};

/**
 * @brief 操作符映射表
 */
class OperatorMap {
public:
    static const std::unordered_map<std::string, TokenType>& getOperators();
    static TokenType getOperatorType(const std::string& op);
    static bool isOperator(const std::string& word);
};

} // namespace CHTL