#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

namespace CHTL {

/**
 * CHTL令牌类型枚举
 */
enum class TokenType {
    // 基本类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
    // 关键字
    USE,            // use
    EXCEPT,         // except
    DELETE,         // delete
    INSERT,         // insert
    INHERIT,        // inherit
    FROM,           // from
    AS,             // as
    AT,             // at
    TOP,            // top
    BOTTOM,         // bottom
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    
    // 模板和自定义关键字
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 模板类型
    TEMPLATE_STYLE,     // @Style
    TEMPLATE_ELEMENT,   // @Element
    TEMPLATE_VAR,       // @Var
    CUSTOM_STYLE,       // @Style (Custom)
    CUSTOM_ELEMENT,     // @Element (Custom)
    CUSTOM_VAR,         // @Var (Custom)
    ORIGIN_HTML,        // @Html
    ORIGIN_STYLE,       // @Style (Origin)
    ORIGIN_JAVASCRIPT,  // @JavaScript
    IMPORT_HTML,        // @Html (Import)
    IMPORT_STYLE,       // @Style (Import)
    IMPORT_JAVASCRIPT,  // @JavaScript (Import)
    IMPORT_CHTL,        // @Chtl
    IMPORT_CJMOD,       // @CJmod
    CONFIG_CONFIG,      // @Config
    
    // HTML元素
    HTML_ELEMENT,   // html, head, body, div, span等
    
    // 特殊节点
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    
    // 操作符
    COLON,          // :
    EQUAL,          // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    QUESTION,       // ?
    EXCLAMATION,    // !
    ARROW,          // ->
    BIND_OP,        // &->
    
    // 括号
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_ANGLE,     // <
    RIGHT_ANGLE,    // >
    
    // 注释
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GENERATOR_COMMENT, // #
    
    // 特殊语法
    SELECTOR_START, // {{
    SELECTOR_END,   // }}
    RESPONSIVE_START, // $JS变量名$
    RESPONSIVE_END,
    
    // 其他
    NEWLINE,        // 换行
    EOF_TOKEN,      // 文件结束
    UNKNOWN         // 未知
};

/**
 * CHTL令牌类
 */
class Token {
public:
    Token(TokenType type, const std::string& value, int line = 0, int column = 0);
    
    TokenType getType() const;
    const std::string& getValue() const;
    int getLine() const;
    int getColumn() const;
    
    std::string toString() const;
    bool isOperator() const;
    bool isKeyword() const;
    bool isBracket() const;
    bool isHTML() const;
    bool isTemplate() const;
    bool isCustom() const;
    bool isOrigin() const;
    bool isImport() const;
    bool isNamespace() const;
    bool isConfiguration() const;

private:
    TokenType type_;
    std::string value_;
    int line_;
    int column_;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H