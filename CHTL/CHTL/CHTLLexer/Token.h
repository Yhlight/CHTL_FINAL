#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

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
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    SEMICOLON,      // ;
    COLON,          // :
    EQUAL,          // =
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    AMPERSAND,      // &
    QUESTION,       // ?
    EXCLAMATION,    // !
    PIPE,           // |
    SLASH,          // /
    BACKSLASH,      // backslash
    ASTERISK,       // *
    PLUS,           // +
    MINUS,          // -
    PERCENT,        // %
    CARET,          // ^
    TILDE,          // ~
    AT,             // @
    DOLLAR,         // $
    UNDERSCORE,     // _
    
    // 运算符
    AND,            // &&
    OR,             // ||
    ARROW,          // ->
    DOUBLE_ARROW,   // =>
    POWER,          // **
    INCREMENT,      // ++
    DECREMENT,      // --
    
    // 比较运算符
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    LESS,           // <
    GREATER,        // >
    
    // 注释
    SINGLE_COMMENT, // //
    MULTI_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
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
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    EXCEPT,         // except
    FROM,           // from
    AS,             // as
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    
    // HTML5
    HTML5,          // html5
    
    // CHTL特有类型
    COLON_EQUAL,    // := (CE对等式)
    ELEMENT_NAME,   // 元素名称
    ATTRIBUTE_NAME, // 属性名称
    ATTRIBUTE_VALUE, // 属性值
    TEXT_CONTENT,   // 文本内容
    UNQUOTED_LITERAL, // 无引号字面量
    SINGLE_QUOTED_LITERAL, // 单引号字面量
    DOUBLE_QUOTED_LITERAL, // 双引号字面量
    
    // 特殊
    END_OF_FILE,    // 文件结束
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    UNKNOWN         // 未知
};

class Token {
public:
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::UNKNOWN, 
          const std::string& v = "", 
          size_t l = 0, 
          size_t c = 0, 
          size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为运算符
    bool isOperator() const;
    
    // 判断是否为分隔符
    bool isDelimiter() const;
    
    // 获取类型名称
    std::string getTypeName() const;
    
    // 转换为字符串
    std::string toString() const;
    
    // 比较操作符
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
};

// Token流类
class TokenStream {
private:
    std::vector<Token> tokens;
    size_t currentIndex;
    
public:
    TokenStream() : currentIndex(0) {}
    
    void addToken(const Token& token);
    void addTokens(const std::vector<Token>& tokens);
    
    Token peek(size_t offset = 0) const;
    Token consume();
    bool hasNext() const;
    size_t size() const;
    size_t getCurrentIndex() const;
    void reset();
    
    // 查找下一个指定类型的token
    Token findNext(TokenType type) const;
    bool hasNext(TokenType type) const;
    
    // 跳过空白字符和注释
    void skipWhitespaceAndComments();
    
    // 获取当前位置的token
    Token current() const;
    
    // 回退一个token
    void backtrack();
};

} // namespace CHTL

#endif // CHTL_TOKEN_H