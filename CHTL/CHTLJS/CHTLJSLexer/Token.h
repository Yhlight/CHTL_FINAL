#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include <string>
#include <vector>

namespace CHTLJS {

enum class TokenType {
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
    ARROW,          // ->
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
    
    // CHTL JS 特有语法
    VIR,            // Vir
    LISTEN,         // Listen
    ANIMATE,        // Animate
    ROUTER,         // Router
    SCRIPT_LOADER,  // ScriptLoader
    DELEGATE,       // Delegate
    
    // 增强选择器
    SELECTOR_START, // {{
    SELECTOR_END,   // }}
    
    // 响应式值
    REACTIVE_START, // $JS变量名$
    REACTIVE_END,   // $
    
    // 关键字
    FUNCTION,       // function
    VAR,            // var
    LET,            // let
    CONST,          // const
    IF,             // if
    ELSE,           // else
    FOR,            // for
    WHILE,          // while
    DO,             // do
    SWITCH,         // switch
    CASE,           // case
    DEFAULT,        // default
    BREAK,          // break
    CONTINUE,       // continue
    RETURN,         // return
    TRY,            // try
    CATCH,          // catch
    FINALLY,        // finally
    THROW,          // throw
    NEW,            // new
    THIS,           // this
    SUPER,          // super
    CLASS,          // class
    EXTENDS,        // extends
    IMPLEMENTS,     // implements
    INTERFACE,      // interface
    ENUM,           // enum
    PUBLIC,         // public
    PRIVATE,        // private
    PROTECTED,      // protected
    STATIC,         // static
    ABSTRACT,       // abstract
    FINAL,          // final
    NATIVE,         // native
    SYNCHRONIZED,   // synchronized
    VOLATILE,       // volatile
    TRANSIENT,      // transient
    STRICTFP,       // strictfp
    
    // 特殊标记
    EOF_TOKEN,      // 文件结束
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    COMMENT,        // 注释
    UNKNOWN         // 未知
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "", 
          size_t l = 0, size_t c = 0, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
    
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
    
    std::string toString() const;
    bool isOperator() const;
    bool isKeyword() const;
    bool isLiteral() const;
    bool isPunctuation() const;
};

class TokenList {
private:
    std::vector<Token> tokens;
    size_t currentIndex;
    
public:
    TokenList() : currentIndex(0) {}
    
    void addToken(const Token& token);
    void addToken(TokenType type, const std::string& value, size_t line, size_t column, size_t position);
    
    Token getCurrent() const;
    Token getNext();
    Token peek(size_t offset = 1) const;
    bool hasNext() const;
    bool hasNext(size_t offset) const;
    
    void advance();
    void reset();
    size_t size() const;
    bool empty() const;
    
    Token operator[](size_t index) const;
    Token at(size_t index) const;
    
    std::vector<Token> getAll() const;
    std::vector<Token> getRange(size_t start, size_t end) const;
    
    void clear();
    void print() const;
};

} // namespace CHTLJS

#endif // CHTLJS_TOKEN_H