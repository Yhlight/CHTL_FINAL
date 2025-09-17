#ifndef CHTLTOKEN_H
#define CHTLTOKEN_H

#include <string>

namespace CHTL {

/**
 * CHTL Token类型枚举
 */
enum class CHTLTokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
    // 关键字
    HTML,           // html
    HEAD,           // head
    BODY,           // body
    TITLE,          // title
    DIV,            // div
    SPAN,           // span
    P,              // p
    A,              // a
    IMG,            // img
    BR,             // br
    HR,             // hr
    UL,             // ul
    OL,             // ol
    LI,             // li
    TABLE,          // table
    TR,             // tr
    TD,             // td
    TH,             // th
    FORM,           // form
    INPUT,          // input
    BUTTON,         // button
    TEXTAREA,       // textarea
    SELECT,         // select
    OPTION,         // option
    SCRIPT,         // script
    STYLE,          // style
    LINK,           // link
    META,           // meta
    
    // CHTL特殊关键字
    TEXT,           // text
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    CONFIGURATION,  // [Configuration]
    NAMESPACE,      // [Namespace]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 模板类型
    TEMPLATE_STYLE, // @Style
    TEMPLATE_ELEMENT, // @Element
    TEMPLATE_VAR,   // @Var
    
    // 自定义类型
    CUSTOM_STYLE,   // @Style
    CUSTOM_ELEMENT, // @Element
    CUSTOM_VAR,     // @Var
    
    // 原始嵌入类型
    ORIGIN_HTML,    // @Html
    ORIGIN_STYLE,   // @Style
    ORIGIN_JAVASCRIPT, // @JavaScript
    
    // 导入类型
    IMPORT_HTML,    // @Html
    IMPORT_STYLE,   // @Style
    IMPORT_JAVASCRIPT, // @JavaScript
    IMPORT_CHTL,    // @Chtl
    IMPORT_CJMOD,   // @CJmod
    
    // 操作符
    COLON,          // :
    EQUAL,          // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    PERCENT,        // %
    AMPERSAND,      // &
    PIPE,           // |
    QUESTION,       // ?
    EXCLAMATION,    // !
    UNDERSCORE,     // _
    MINUS,          // -
    ARROW,          // ->
    
    // 括号
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_ANGLE,     // <
    RIGHT_ANGLE,    // >
    
    // 特殊符号
    DOLLAR,         // $ 响应式值
    AT,             // @
    
    // 关键字
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
    
    // 其他
    NEWLINE,        // 换行符
    WHITESPACE,     // 空白字符
    COMMENT,        // 注释
    GENERATOR_COMMENT, // # 注释
    UNKNOWN,        // 未知
    EOF_TOKEN       // 文件结束
};

/**
 * CHTL Token类
 */
class CHTLToken {
public:
    CHTLToken(CHTLTokenType type, const std::string& value, size_t line, size_t column);
    
    CHTLTokenType getType() const;
    std::string getValue() const;
    size_t getLine() const;
    size_t getColumn() const;
    
    std::string toString() const;
    bool isOperator() const;
    bool isKeyword() const;
    bool isLiteral() const;
    bool isPunctuation() const;
    bool isHTMLTag() const;
    bool isCHTLKeyword() const;

private:
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
};

} // namespace CHTL

#endif // CHTLTOKEN_H