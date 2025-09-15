#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace CHTLJS {

/**
 * 令牌类型枚举
 */
enum class TokenType {
    // 基本类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
    // 关键字
    SCRIPT_LOADER,  // ScriptLoader
    LISTEN,         // Listen
    ANIMATE,        // Animate
    ROUTER,         // Router
    VIR,            // Vir
    INEVERAWAY,     // iNeverAway
    UTIL,           // util
    PRINTMYLOVE,    // printMylove
    
    // 操作符
    ARROW,          // ->
    BIND_OP,        // &->
    DOT,            // .
    COMMA,          // ,
    SEMICOLON,      // ;
    COLON,          // :
    EQUAL,          // =
    QUESTION,       // ?
    EXCLAMATION,    // !
    
    // 括号
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    
    // 增强选择器
    SELECTOR_START, // {{
    SELECTOR_END,   // }}
    
    // 响应式值
    RESPONSIVE_START, // $JS变量名$
    RESPONSIVE_END,
    
    // 特殊
    NEWLINE,        // 换行
    EOF_TOKEN,      // 文件结束
    UNKNOWN         // 未知
};

/**
 * 令牌类
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

private:
    TokenType type_;
    std::string value_;
    int line_;
    int column_;
};

} // namespace CHTLJS

#endif // TOKEN_H