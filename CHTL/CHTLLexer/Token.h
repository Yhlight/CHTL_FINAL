#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 字面量（无修饰字符串）
    
    // 符号
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
    AT,             // @
    AMPERSAND,      // &
    QUESTION,       // ?
    EXCLAMATION,    // !
    PIPE,           // |
    TILDE,          // ~
    CARET,          // ^
    DOLLAR,         // $
    PERCENT,        // %
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /
    BACKSLASH,      // \
    LESS_THAN,      // <
    GREATER_THAN,   // >
    
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
    STYLE,          // @Style
    ELEMENT,        // @Element
    VAR,            // @Var
    HTML,           // @Html
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    
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
    SCRIPT,         // script
    
    // 注释
    COMMENT,        // //
    MULTILINE_COMMENT, // /* */
    GENERATOR_COMMENT, // #
    
    // 特殊
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误标记
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token() : type(TokenType::ERROR), line(0), column(0), position(0) {}
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isIdentifier() const;
    
    std::string toString() const;
};

// 关键字映射
class KeywordMap {
public:
    static TokenType getKeywordType(const std::string& keyword);
    static bool isKeyword(const std::string& word);
    static std::vector<std::string> getAllKeywords();
    
private:
    static const std::map<std::string, TokenType> keywordMap_;
};

} // namespace CHTL