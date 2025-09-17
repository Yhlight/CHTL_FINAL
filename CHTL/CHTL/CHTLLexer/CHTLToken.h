#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class CHTLTokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
    // 运算符
    ASSIGN,         // =
    COLON,          // :
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    QUESTION,       // ?
    EXCLAMATION,    // !
    
    // 比较运算符
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    GREATER,        // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    
    // 算术运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    
    // 逻辑运算符
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // 括号
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    
    // 特殊符号
    AT,             // @
    HASH,           // #
    DOLLAR,         // $
    UNDERSCORE,     // _
    TILDE,          // ~
    PIPE,           // |
    AMPERSAND,      // &
    
    // CHTL 特有语法
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
    
    // 原始嵌入类型
    ORIGIN_HTML,        // @Html
    ORIGIN_STYLE,       // @Style
    ORIGIN_JAVASCRIPT,  // @JavaScript
    
    // 导入类型
    IMPORT_HTML,        // @Html
    IMPORT_STYLE,       // @Style
    IMPORT_JAVASCRIPT,  // @JavaScript
    IMPORT_CHTL,        // @Chtl
    IMPORT_CJMOD,       // @CJmod
    
    // 关键字
    KEYWORD_INHERIT,    // inherit
    KEYWORD_DELETE,     // delete
    KEYWORD_INSERT,     // insert
    KEYWORD_AFTER,      // after
    KEYWORD_BEFORE,     // before
    KEYWORD_REPLACE,    // replace
    KEYWORD_ATTOP,      // at top
    KEYWORD_ATBOTTOM,   // at bottom
    KEYWORD_FROM,       // from
    KEYWORD_AS,         // as
    KEYWORD_EXCEPT,     // except
    KEYWORD_USE,        // use
    KEYWORD_HTML5,      // html5
    KEYWORD_TEXT,       // text
    KEYWORD_STYLE,      // style
    KEYWORD_SCRIPT,     // script
    
    // HTML元素
    HTML_ELEMENT,       // html, head, body, div, span, etc.
    
    // 特殊标记
    EOF_TOKEN,      // 文件结束
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    COMMENT,        // 注释
    GENERATOR_COMMENT, // # 注释
    UNKNOWN         // 未知
};

struct CHTLToken {
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLToken(CHTLTokenType t = CHTLTokenType::UNKNOWN, const std::string& v = "", 
              size_t l = 0, size_t c = 0, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool operator==(const CHTLToken& other) const {
        return type == other.type && value == other.value;
    }
    
    bool operator!=(const CHTLToken& other) const {
        return !(*this == other);
    }
    
    std::string toString() const;
    bool isOperator() const;
    bool isKeyword() const;
    bool isLiteral() const;
    bool isPunctuation() const;
    bool isHTML() const;
};

class CHTLTokenList {
private:
    std::vector<CHTLToken> tokens;
    size_t currentIndex;
    
public:
    CHTLTokenList() : currentIndex(0) {}
    
    void addToken(const CHTLToken& token);
    void addToken(CHTLTokenType type, const std::string& value, size_t line, size_t column, size_t position);
    
    CHTLToken getCurrent() const;
    CHTLToken getNext();
    CHTLToken peek(size_t offset = 1) const;
    bool hasNext() const;
    bool hasNext(size_t offset) const;
    
    void advance();
    void reset();
    size_t size() const;
    bool empty() const;
    
    CHTLToken operator[](size_t index) const;
    CHTLToken at(size_t index) const;
    
    std::vector<CHTLToken> getAll() const;
    std::vector<CHTLToken> getRange(size_t start, size_t end) const;
    
    void clear();
    void print() const;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H