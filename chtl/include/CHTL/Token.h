#pragma once

#include <string>
#include <variant>

namespace CHTL {

/**
 * Token类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 字面量
    
    // 符号
    LBRACE,         // {
    RBRACE,         // }
    LPAREN,         // (
    RPAREN,         // )
    LBRACKET,       // [
    RBRACKET,       // ]
    SEMICOLON,      // ;
    COLON,          // :
    EQUALS,         // =
    COMMA,          // ,
    DOT,            // .
    SLASH,          // /
    BACKSLASH,      // backslash
    ASTERISK,       // *
    PLUS,           // +
    MINUS,          // -
    PERCENT,        // %
    EXCLAMATION,    // !
    QUESTION,       // ?
    AMPERSAND,      // &
    PIPE,           // |
    LESS,           // <
    GREATER,        // >
    
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
    
    // 特殊类型
    COMMENT,        // 注释
    WHITESPACE,     // 空白字符
    NEWLINE,        // 换行符
    EOF_TOKEN,      // 文件结束
    
    // HTML元素
    HTML,           // html
    HEAD,           // head
    BODY,           // body
    DIV,            // div
    SPAN,           // span
    P,              // p
    A,              // a
    IMG,            // img
    BR,             // br
    HR,             // hr
    H1, H2, H3, H4, H5, H6,  // 标题
    UL, OL, LI,     // 列表
    TABLE, TR, TD, TH,  // 表格
    FORM, INPUT, BUTTON,  // 表单
    SCRIPT_TAG,     // script标签
    STYLE_TAG,      // style标签
    
    // 样式相关
    CLASS,          // class
    ID,             // id
    WIDTH,          // width
    HEIGHT,         // height
    COLOR,          // color
    BACKGROUND,     // background
    MARGIN,         // margin
    PADDING,        // padding
    BORDER,         // border
    FONT,           // font
    TEXT_ALIGN,     // text-align
    DISPLAY,        // display
    POSITION,       // position
    FLOAT,          // float
    CLEAR,          // clear
    OVERFLOW,       // overflow
    Z_INDEX,        // z-index
    OPACITY,        // opacity
    TRANSFORM,      // transform
    TRANSITION,     // transition
    ANIMATION,      // animation
    
    // 单位
    PX,             // px
    EM,             // em
    REM,            // rem
    PERCENT_UNIT,   // %
    VW,             // vw
    VH,             // vh
    
    // 颜色
    RED,            // red
    GREEN,          // green
    BLUE,           // blue
    WHITE,          // white
    BLACK,          // black
    YELLOW,         // yellow
    ORANGE,         // orange
    PURPLE,         // purple
    PINK,           // pink
    GRAY,           // gray
    TRANSPARENT,    // transparent
    
    // 位置
    LEFT,           // left
    RIGHT,          // right
    CENTER,         // center
    TOP_POS,        // top
    BOTTOM_POS,     // bottom
    MIDDLE,         // middle
    
    // 显示
    BLOCK,          // block
    INLINE,         // inline
    INLINE_BLOCK,   // inline-block
    FLEX,           // flex
    GRID,           // grid
    NONE,           // none
    
    // 其他
    AUTO,           // auto
    INHERIT_VALUE,  // inherit
    INITIAL,        // initial
    UNSET,          // unset
    IMPORTANT,      // !important
};

/**
 * Token类
 * 表示词法分析的结果
 */
class Token {
public:
    Token(TokenType type, const std::string& value, size_t line, size_t column);
    
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    bool isType(TokenType type) const { return type_ == type; }
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    
    std::string toString() const;
    
private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

} // namespace CHTL