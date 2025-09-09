#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace CHTL {

enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 无修饰字面量
    
    // 分隔符
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COLON,          // :
    EQUALS,         // =
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    AMPERSAND,      // &
    ARROW,          // ->
    
    // 运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    LESS,           // <
    GREATER,        // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    AND,            // &&
    OR,             // ||
    NOT,            // !
    QUESTION,       // ?
    
    // 关键字
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 类型关键字
    AT_STYLE,       // @Style
    AT_ELEMENT,     // @Element
    AT_VAR,         // @Var
    AT_HTML,        // @Html
    AT_JAVASCRIPT,  // @JavaScript
    AT_CHTL,        // @Chtl
    AT_CONFIG,      // @Config
    
    // 操作关键字
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    FROM,           // from
    AS,             // as
    EXCEPT,         // except
    USE,            // use
    HTML5,          // html5
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    
    // 注释
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
    // 特殊
    END_OF_FILE,    // 文件结束
    UNKNOWN         // 未知
};

class Token {
public:
    Token(TokenType type, std::string_view value, size_t line, size_t column)
        : type_(type), value_(value), line_(line), column_(column) {}
    
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    bool isType(TokenType type) const { return type_ == type; }
    bool isKeyword() const;
    bool isOperator() const;
    bool isSeparator() const;
    
    std::string toString() const;
    
private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

// 关键字映射
class KeywordMap {
public:
    static const std::unordered_map<std::string, TokenType>& getKeywords();
    static TokenType getKeywordType(const std::string& keyword);
    static bool isKeyword(const std::string& word);
};

} // namespace CHTL