#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace CHTLJS {

/**
 * Token类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
    // 关键字
    VIR,            // 虚对象关键字
    LISTEN,         // 监听器关键字
    ANIMATE,        // 动画关键字
    ROUTER,         // 路由关键字
    SCRIPT_LOADER,  // 脚本加载器关键字
    DELEGATE,       // 事件委托关键字
    INEVERAWAY,     // iNeverAway关键字
    UTIL,           // util关键字
    CHANGE,         // change关键字
    THEN,           // then关键字
    
    // 操作符
    ARROW,          // -> 操作符
    EVENT_ARROW,    // &-> 操作符
    DOT,            // . 操作符
    COMMA,          // , 操作符
    COLON,          // : 操作符
    SEMICOLON,      // ; 操作符
    EQUAL,          // = 操作符
    QUESTION,       // ? 操作符
    EXCLAMATION,    // ! 操作符
    UNDERSCORE,     // _ 操作符
    
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
    HASH,           // # 选择器
    PERCENT,        // % 选择器
    PIPE,           // | 选择器
    AMPERSAND,      // & 引用选择器
    
    // 其他
    NEWLINE,        // 换行符
    WHITESPACE,     // 空白字符
    COMMENT,        // 注释
    UNKNOWN,        // 未知
    EOF_TOKEN       // 文件结束
};

/**
 * Token类
 * 表示词法分析器识别出的词法单元
 */
class Token {
public:
    Token(TokenType type, const std::string& value, size_t line, size_t column);
    
    TokenType getType() const;
    std::string getValue() const;
    size_t getLine() const;
    size_t getColumn() const;
    
    std::string toString() const;
    bool isOperator() const;
    bool isKeyword() const;
    bool isLiteral() const;
    bool isPunctuation() const;

private:
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};

} // namespace CHTLJS

#endif // TOKEN_H